#include "enginepch.h"
#include "Engine/Asset/Serializer/Serializer.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/Scene.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Log.h"
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
struct glz::meta<engine::Mesh> { static constexpr auto value = [](auto& mesh) -> uint64_t& { return reinterpret_cast<uint64_t&>(mesh); }; };

template <>
struct glz::meta<engine::Material> { static constexpr auto value = [](auto& material) -> uint64_t& { return reinterpret_cast<uint64_t&>(material); }; };

template <>
struct glz::meta<engine::MaterialInstance> {
	using T = engine::MaterialInstance;
	static constexpr auto value = [](auto& m) -> decltype(m.id)& { return m.id; };
};

namespace engine
{
	struct HierarchyJSON
	{
		Uuid parent;
	};

	struct ColliderComponentJSON
	{
		Uuid mesh;
		uint32_t type = 0;
		bool isTrigger = false;
	};

	struct CameraPerspectiveJSON
	{
		float fov = 0.0f, near = 0.0f, far = 0.0f;
	};

	struct CameraOrthographicJSON
	{
		float size = 0.0f, near = 0.0f, far = 0.0f;
	};

	struct CameraComponentJSON
	{
		uint32_t projection = 0;
		CameraPerspectiveJSON perspective;
		CameraOrthographicJSON orthographic;
		uint32_t background = 0;
		glm::vec4 clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		bool primary = false;
	};

	struct LightComponentJSON
	{
		uint32_t type = 0;
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
		float range = 1.0f;
		float angle = 1.0f;
	};

	struct ScriptComponentJSON
	{
		uint32_t script;
		std::map<std::string, glz::generic> fields;
	};

	struct EntityJSON
	{
		Uuid id;
		std::string_view name;
		bool enabled = true;
		HierarchyJSON hierarchy;

		std::optional<TransformComponent> transform;
		std::optional<MeshComponent> mesh;
		std::optional<ColliderComponentJSON> collider;
		std::optional<CameraComponentJSON> camera;
		std::optional<LightComponentJSON> light;
		std::optional<ScriptComponentJSON> script;
	};
}

namespace engine
{
	namespace
	{
		std::string buffer;
	}

	void SerializeEntityRecursive(const entt::registry& registry, entt::entity entity, std::vector<EntityJSON>& json)
	{
		EntityJSON eJson;

		eJson.enabled = !registry.any_of<DisabledTag>(entity);
		eJson.id = registry.get<IdentityComponent>(entity).id;
		eJson.name = registry.get<NameComponent>(entity).name;

		// Transform
		if (registry.any_of<TransformComponent>(entity))
		{
			eJson.transform = registry.get<TransformComponent>(entity);
			eJson.transform->rotation = glm::degrees(eJson.transform->rotation);
		}

		// Hierarchy
		{
			const auto& hc = registry.get<HierarchyComponent>(entity);
			if (hc.parent != entt::null)
			{
				const auto& parentIdentity = registry.get<IdentityComponent>(hc.parent);
				eJson.hierarchy = HierarchyJSON{ parentIdentity.id };
			}
		}

		// Mesh
		if (registry.any_of<MeshComponent>(entity))
		{
			const auto& mc = registry.get<MeshComponent>(entity);
			eJson.mesh = mc;
		}

		// Collider
		if (registry.any_of<ColliderComponent>(entity))
		{
			const auto& cc = registry.get<ColliderComponent>(entity);
			eJson.collider = ColliderComponentJSON{ cc.collisionMesh.id, static_cast<uint32_t>(cc.type), cc.isTrigger };
		}

		// Camera
		if (registry.any_of<CameraComponent>(entity))
		{
			const CameraComponent& cam = registry.get<CameraComponent>(entity);
			CameraComponentJSON cJson;
			cJson.projection = static_cast<uint32_t>(cam.projectionType);
			//cJson.aspectRatio = cam.GetAspectRatio();

			cJson.perspective = { cam.perspective.fov, cam.perspective.nearClip, cam.perspective.farClip };
			cJson.orthographic = { cam.orthographic.size, cam.orthographic.nearClip, cam.orthographic.farClip };

			cJson.background = static_cast<uint32_t>(cam.backgroundMode);
			cJson.clearColor = cam.clearColor;

			cJson.primary = registry.any_of<PrimaryCameraTag>(entity);
			eJson.camera = cJson;
		}

		// Light
		//if (registry.any_of<LightComponent>(entity))
		//{
		//	const auto& l = registry.get<LightComponent>(entity);
		//	eJson.LightComponent = LightComponentJSON{
		//		static_cast<uint32_t>(l.type), l.color, l.intensity, l.range, l.angle
		//	};
		//}

		json.push_back(std::move(eJson));

		auto& hc = registry.get<HierarchyComponent>(entity);
		for (entt::entity child : hc.children)
		{
			SerializeEntityRecursive(registry, child, json);
		}
	}

	void Serializer::SerializeScene(Scene scene, const std::filesystem::path& path)
	{
		if (path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' extension in save path: '{}'", path);
			return;
		}

		const auto& sceneData = scene.Get();
		const entt::registry& registry = sceneData.registry;

		std::vector<EntityJSON> json;

		for (const entt::entity root : sceneData.rootEntities)
		{
			SerializeEntityRecursive(registry, root, json);
		}

		if (auto ec = glz::write_file_json < glz::opts{ .prettify = true } > (json, path.string(), buffer))
		{
			LOG_ERROR("Failed to save scene file '{}': {}", path, glz::format_error(ec));
		}
	}

	SceneAsset Serializer::DeserializeScene(const std::filesystem::path& path)
	{
		if (path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' extension, got '{}'", path.extension().string());
			return SceneAsset{};
		}

		std::vector<EntityJSON> json;

		std::map<Uuid, Uuid> parentMap; // child ID -> parent ID

		if (auto ec = glz::read_file_json(json, path.string(), buffer))
		{
			LOG_ERROR("Failed to load scene file '{}': {}", path, glz::format_error(ec, buffer));
			return SceneAsset{};
		}

		SceneAsset scene;
		scene.environmentTexture = Texture2D::HDR; //TODO: store in scene file

		entt::registry& registry = scene.registry;

		for (const EntityJSON& eJson : json)
		{
			Uuid id = eJson.id;

			if (!id)
			{
				LOG_WARNING("Entity has invalid ID, skipping");
				continue;
			}

			entt::entity handle = registry.create();

			// identity
			{
				registry.emplace<IdentityComponent>(handle, id);
				scene.entityMap[id] = handle;
			}

			// disabled tag
			if (!eJson.enabled)
			{
				registry.emplace<DisabledTag>(handle);
			}

			// name
			registry.emplace<NameComponent>(handle).name = eJson.name;

			// transform
			if(eJson.transform.has_value())
			{
				auto& tc = registry.emplace<TransformComponent>(handle);
				tc = eJson.transform.value();
				tc.rotation = glm::radians(tc.rotation);

				registry.emplace<WorldTransformComponent>(handle);
				registry.emplace<TransformDirty>(handle);
			}

			// hierarchy
			registry.emplace<HierarchyComponent>(handle);
			if (eJson.hierarchy.parent)
			{
				parentMap[id] = eJson.hierarchy.parent;
			}
			else
			{
				scene.rootEntities.push_back(handle);
			}

			// mesh
			if (eJson.mesh.has_value())
			{
				auto& mc = registry.emplace<MeshComponent>(handle);
				mc = eJson.mesh.value();
			}

			// collider
			if (eJson.collider.has_value())
			{
				auto& cComp = registry.emplace<ColliderComponent>(handle);
				const auto& cJson = eJson.collider.value();
				cComp.collisionMesh = Mesh{ cJson.mesh };
				cComp.type = static_cast<BodyType>(cJson.type);
				cComp.isTrigger = cJson.isTrigger;
			}

			// Camera
			//TODO: use camera component directly, instead of this json struct
			if (eJson.camera.has_value())
			{
				auto& cComp = registry.emplace<CameraComponent>(handle);
				const auto& cJson = eJson.camera.value();

				cComp.projectionType = static_cast<CameraComponent::Projection>(cJson.projection);
				cComp.perspective.fov = cJson.perspective.fov;
				cComp.perspective.nearClip = cJson.perspective.near;
				cComp.perspective.farClip = cJson.perspective.far;
				cComp.orthographic.size = cJson.orthographic.size;
				cComp.orthographic.nearClip = cJson.orthographic.near;
				cComp.orthographic.farClip = cJson.orthographic.far;
				cComp.backgroundMode = static_cast<CameraComponent::Background>(cJson.background);
				cComp.clearColor = cJson.clearColor;

				if(cJson.primary)
				{
					registry.emplace_or_replace<PrimaryCameraTag>(handle);
				}

				registry.emplace_or_replace<CameraProjectionDirty>(handle);
				registry.emplace_or_replace<CameraViewDirty>(handle);
			}

			// light
			if (eJson.light.has_value())
			{
				auto& lComp = registry.emplace<LightComponent>(handle);
				const auto& lJson = eJson.light.value();
				lComp.type = static_cast<LightType>(lJson.type);
				lComp.color = lJson.color;
				lComp.intensity = lJson.intensity;
				lComp.range = lJson.range;
				lComp.angle = lJson.angle;
			}
		}

		// set parent-child relationships
		for (const auto& [childId, parentId] : parentMap)
		{
			entt::entity child = scene.entityMap.at(childId);
			entt::entity parent = scene.entityMap.at(parentId);

			auto& childHC = registry.get<HierarchyComponent>(child);
			childHC.parent = parent;

			auto& parentHC = registry.get<HierarchyComponent>(parent);
			parentHC.children.push_back(child);
		}

		return scene;
	}
}