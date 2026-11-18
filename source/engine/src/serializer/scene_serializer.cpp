#include "enginepch.h"
#include "engine/serializer/serializer.h"
#include "engine/core/components.h"
#include "engine/core/scene.h"
#include <glaze/glaze.hpp>
#include <glm/glm.hpp>

template <> struct glz::meta<glm::ivec2> { static constexpr auto value = glz::array(&glm::ivec2::x, &glm::ivec2::y); };

template <>
struct glz::meta<glm::vec2> { static constexpr auto value = array(&glm::vec2::x, &glm::vec2::y); };

template <>
struct glz::meta<glm::vec3> { static constexpr auto value = array(&glm::vec3::x, &glm::vec3::y, &glm::vec3::z); };

template <>
struct glz::meta<glm::vec4> { static constexpr auto value = array(&glm::vec4::x, &glm::vec4::y, &glm::vec4::z, &glm::vec4::w); };

template <>
struct glz::meta<engine::Uuid> { static constexpr auto value = [](auto& uuid) -> uint64_t& { return reinterpret_cast<uint64_t&>(uuid); }; };

template <>
struct glz::meta<engine::MeshHandle> { static constexpr auto value = [](auto& mesh) -> uint64_t& { return reinterpret_cast<uint64_t&>(mesh); }; };

template <>
struct glz::meta<engine::MaterialHandle> { static constexpr auto value = [](auto& material) -> uint64_t& { return reinterpret_cast<uint64_t&>(material); }; };

template <>
struct glz::meta<engine::MaterialInstanceHandle> {
	using T = engine::MaterialInstanceHandle;
	static constexpr auto value = [](auto& m) -> decltype(m.id)& { return m.id; };
};

template <>
struct glz::meta<engine::component::Camera>
{
	using T = engine::component::Camera;
	static constexpr auto value = glz::object(
		"projection", &T::projection,
		"perspective", &T::perspective,
		"orthographic", &T::orthographic,
		"backgroundMode", &T::backgroundMode,
		"clearColor", &T::clearColor
	);
};

namespace engine
{
	struct EntityJSON
	{
		Uuid id;
		std::string_view name;
		bool enabled;
		std::optional<Uuid> parent;
		std::optional<component::Transform> transform;
		std::optional<component::Mesh> mesh;
		std::optional<component::Camera> camera;
	};

	struct SceneJson
	{
		Uuid primaryCameraEntity;
		std::vector<EntityJSON> entities;
	};

	namespace
	{
		std::string buffer;
	}

	void SerializeEntityRecursive(const entt::registry& registry, entt::entity entity, SceneJson& json)
	{
		if (entity == entt::null) return;

		EntityJSON eJson;

		eJson.enabled = !registry.any_of<tag::Disabled>(entity);
		eJson.id = registry.get<component::Identity>(entity).id;
		eJson.name = registry.get<component::Name>(entity).name;

		auto& rc = registry.get<component::Relationship>(entity);
		if (rc.parent != entt::null)
		{
			eJson.parent = registry.get<component::Identity>(rc.parent).id;
		}

		// Transform
		if(auto* transform = registry.try_get<component::Transform>(entity))
		{
			eJson.transform = *transform;
			eJson.transform->rotation = glm::degrees(eJson.transform->rotation);
		}

		// Mesh
		if (auto* mesh = registry.try_get<component::Mesh>(entity))
		{
			eJson.mesh = *mesh;
		}

		// Camera
		if (auto* camera = registry.try_get<component::Camera>(entity))
		{
			eJson.camera = *camera;
		}

		json.entities.push_back(std::move(eJson));

		SerializeEntityRecursive(registry, rc.firstChild, json);
		SerializeEntityRecursive(registry, rc.nextSibling, json);
	}

	void serializer::SerializeScene(SceneHandle handle, const std::filesystem::path& path)
	{
		if (path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' extension in save path: '{}'", path);
			return;
		}

		SceneJson json;

		const engine::Scene& scene = handle.Get();

		if(scene.primaryCameraEntity != entt::null)
		{
			json.primaryCameraEntity = scene.registry.get<component::Identity>(scene.primaryCameraEntity).id;
		}

		SerializeEntityRecursive(scene.registry, scene.firstRoot, json);

		if (auto ec = glz::write_file_json < glz::opts{ .prettify = true } > (json, path.string(), buffer))
		{
			LOG_ERROR("Failed to save scene file '{}': {}", path, glz::format_error(ec));
		}
	}

	Scene serializer::DeserializeScene(const std::filesystem::path& path)
	{
		if (path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' extension, got '{}'", path.extension().string());
			return Scene{};
		}

		SceneJson json;

		if (auto ec = glz::read_file_json(json, path.string(), buffer))
		{
			LOG_ERROR("Failed to load scene file '{}': {}", path, glz::format_error(ec, buffer));
			return Scene{};
		}

		Scene scene;

		std::unordered_map<engine::Uuid, entt::entity> entityMap;
		std::map<Uuid, Uuid> parentMap; // child ID -> parent ID

		entt::registry& registry = scene.registry;

		for (const EntityJSON& eJson : json.entities)
		{
			Uuid id = eJson.id;

			if (!id)
			{
				LOG_WARNING("Entity has invalid ID, skipping");
				continue;
			}

			entt::entity handle = registry.create();

			// identity
			registry.emplace<component::Identity>(handle, id);
			entityMap[id] = handle;

			// disabled tag
			if (!eJson.enabled)
			{
				registry.emplace<tag::Disabled>(handle);
			}

			// name
			registry.emplace<component::Name>(handle).name = eJson.name;

			// transform
			if (eJson.transform.has_value())
			{
				auto& tc = registry.emplace<component::Transform>(handle);
				tc = eJson.transform.value();
				tc.rotation = glm::radians(tc.rotation);

				registry.emplace<component::WorldTransform>(handle);
				registry.emplace<tag::TransformDirty>(handle);
			}

			// relationship
			registry.emplace<component::Relationship>(handle);
			if (eJson.parent.has_value() && static_cast<bool>(eJson.parent.value()) == true)
			{
				parentMap[id] = eJson.parent.value();
			}

			// mesh
			if (eJson.mesh.has_value())
			{
				auto& mc = registry.emplace<component::Mesh>(handle);
				mc = eJson.mesh.value();
			}

			// camera
			if (eJson.camera.has_value())
			{
				auto& cc = registry.emplace<component::Camera>(handle);
				cc =  eJson.camera.value();

				registry.emplace_or_replace<tag::CameraProjectionDirty>(handle);
				registry.emplace_or_replace<tag::CameraViewDirty>(handle);
			}
		}

		// set parent-child relationships
		for (const auto& [childId, parentId] : parentMap)
		{
			entt::entity child = entityMap.at(childId);
			entt::entity parent = entityMap.at(parentId);

			scene.AttachChild(parent, child);
		}

		if(!json.entities.empty())
		{
			scene.firstRoot = entityMap.at(json.entities.at(0).id);
			scene.lastRoot = entityMap.at(json.entities.at(json.entities.size() - 1).id);
		}

		if(json.primaryCameraEntity)
		{
			scene.primaryCameraEntity = entityMap.at(json.primaryCameraEntity);
		}

		return scene;
	}
}