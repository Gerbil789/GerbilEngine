#include "enginepch.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Script/Script.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Core/Components.h"
#include <glaze/glaze.hpp>

template <>
struct glz::meta<glm::vec2> { static constexpr auto value = array(&glm::vec2::x, &glm::vec2::y); };

template <>
struct glz::meta<glm::vec3> { static constexpr auto value = array(&glm::vec3::x, &glm::vec3::y, &glm::vec3::z); };

template <>
struct glz::meta<glm::vec4> { static constexpr auto value = array(&glm::vec4::x, &glm::vec4::y, &glm::vec4::z, &glm::vec4::w); };

template <>
struct glz::meta<Engine::Uuid> { static constexpr auto value = [](auto& uuid) -> uint64_t& { return reinterpret_cast<uint64_t&>(uuid); }; };

template <>
struct glz::meta<Engine::Mesh> { static constexpr auto value = [](auto& mesh) -> uint64_t& { return reinterpret_cast<uint64_t&>(mesh); }; };

template <>
struct glz::meta<Engine::Material> { static constexpr auto value = [](auto& material) -> uint64_t& { return reinterpret_cast<uint64_t&>(material); }; };

namespace Engine
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
		float aspectRatio = 1.0f;
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

namespace Engine
{
	namespace
	{
		std::string buffer;
	}

	void SerializeEntityRecursive(entt::registry& registry, entt::entity entity, std::vector<EntityJSON>& outList)
	{
		EntityJSON eJson;

		eJson.enabled = !registry.any_of<DisabledTag>(entity);
		eJson.id = registry.get<IdentityComponent>(entity).id;
		eJson.name = registry.get<NameComponent>(entity).name;

		// Transform
		if (registry.any_of<TransformComponent>(entity))
		{
			eJson.transform = registry.get<TransformComponent>(entity);
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

			cJson.background = static_cast<uint32_t>(cam.background);
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

		// Script
		if (auto* component = registry.try_get<ScriptComponent>(entity))
		{
			ScriptComponentJSON sJson;
			sJson.script = component->id;

			const auto& desc = Engine::ScriptRegistry::GetDescriptor(component->id);
			auto* instance = component->instance;
			auto& fields = sJson.fields;

			for (const auto& field : desc.fields)
			{
				auto& dst = fields.try_emplace(field.name).first->second;

				switch (field.type)
				{
				case ScriptFieldType::Bool:
					dst = field.GetValue<bool>(instance);
					break;

				case ScriptFieldType::Int:
					dst = field.GetValue<int>(instance);
					break;

				case ScriptFieldType::Float:
					dst = field.GetValue<float>(instance);
					break;

				case ScriptFieldType::Texture:
					dst = std::to_string(static_cast<uint64_t>(field.GetValue<Texture2D>(instance).id));
					break;
				case ScriptFieldType::AudioClip:
					dst = std::to_string(static_cast<uint64_t>(field.GetValue<AudioClip>(instance).id));
					break;
				case ScriptFieldType::Mesh:
					dst = std::to_string(static_cast<uint64_t>(field.GetValue<Mesh>(instance).id));
					break;
				case ScriptFieldType::Material:
					dst = std::to_string(static_cast<uint64_t>(field.GetValue<Material>(instance).id));
					break;

				default:
					LOG_WARNING("Unsupported script field type for serialization: {}",
						static_cast<uint32_t>(field.type));
					break;
				}
			}

			eJson.script = std::move(sJson);
		}

		outList.push_back(std::move(eJson));

		auto& hc = registry.get<HierarchyComponent>(entity);
		for (entt::entity child : hc.children)
		{
			SerializeEntityRecursive(registry, child, outList);
		}
	}

	void SceneSerializer::Serialize(Scene scene, const std::filesystem::path& path)
	{
		auto& sceneAsset = AssetManager::GetAsset(scene);

		if (path.extension() != ".json" && path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' or '.json' extension in save path: '{}'", path);
			return;
		}

		entt::registry& registry = sceneAsset.GetRegistry();

		std::vector<EntityJSON> sceneData;

		const auto& roots = sceneAsset.GetRootEntities();
		for (entt::entity root : roots)
		{
			SerializeEntityRecursive(registry, root, sceneData);
		}


		if (auto ec = glz::write_file_json < glz::opts{ .prettify = true } > (sceneData, path.string(), buffer))
		{
			LOG_ERROR("Failed to save scene file '{}': {}", path.string(), glz::format_error(ec));
		}
	}

	std::optional<SceneAsset> SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		if (path.extension() != ".json" && path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' or '.json' extension, got '{}'", path.extension().string());
			return std::nullopt;
		}

		std::vector<EntityJSON> json;

		std::map<Uuid, Uuid> parentMap; // child ID -> parent ID

		if (auto ec = glz::read_file_json(json, path.string(), buffer))
		{
			LOG_ERROR("Failed to load scene file '{}': {}", path, glz::format_error(ec, buffer));
			return std::nullopt;
		}

		SceneAsset scene;
		scene.SetEnvironmentTexture(RESOURCES::TEXTURE::HDR); //TODO: store in scene file

		entt::registry& registry = scene.GetRegistry();

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
				scene.m_EntityMap[id] = handle;
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
				scene.m_RootEntities.push_back(handle);
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
					//cComp.camera.SetAspectRatio(cJson.aspectRatio);
				cComp.perspective.fov = cJson.perspective.fov;
				cComp.perspective.nearClip = cJson.perspective.near;
				cComp.perspective.farClip = cJson.perspective.far;
				cComp.orthographic.size = cJson.orthographic.size;
				cComp.orthographic.nearClip = cJson.orthographic.near;
				cComp.orthographic.farClip = cJson.orthographic.far;
				cComp.background = static_cast<CameraComponent::Background>(cJson.background);
				cComp.clearColor = cJson.clearColor;

				if(cJson.primary)
				{
					registry.emplace_or_replace<PrimaryCameraTag>(handle);
				}
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

			// script
			if (eJson.script.has_value())
			{
				auto& sComp = registry.emplace<ScriptComponent>(handle);
				const auto& sJson = eJson.script.value();

				const Engine::ScriptDescriptor& desc = Engine::ScriptRegistry::GetDescriptor(sJson.script);

				sComp.id = sJson.script;
				sComp.instance = desc.factory();
				sComp.instance->m_Entity = Entity{ handle, &scene }; //TODO: is scene valid after this functin?
				sComp.instance->OnCreate();

				for (const auto& field : desc.fields)
				{
					if (sJson.fields.find(field.name) == sJson.fields.end()) continue;

					const auto& node = sJson.fields.at(field.name);

					switch (field.type)
					{
					case ScriptFieldType::Bool:
						if (node.is_boolean()) field.SetValue<bool>(sComp.instance, node.get_boolean());
						break;
					case ScriptFieldType::Int:
						if (node.is_number()) field.SetValue<int>(sComp.instance, static_cast<int>(static_cast<uint64_t>(node.get_number())));
						break;
					case ScriptFieldType::Float:
						if (node.is_number()) field.SetValue<float>(sComp.instance, static_cast<float>(node.get_number()));
						break;

					case ScriptFieldType::Texture:
						if (node.is_string())
							field.SetValue<Texture2D>(sComp.instance, Texture2D{ Uuid{ std::stoull(node.get_string()) } });
						break;

					case ScriptFieldType::AudioClip:
						if (node.is_string())
							field.SetValue<AudioClip>(sComp.instance, AudioClip{ Uuid{ std::stoull(node.get_string()) } });
						break;

					case ScriptFieldType::Mesh:
						if (node.is_string())
							field.SetValue<Mesh>(sComp.instance, Mesh{ Uuid{ std::stoull(node.get_string()) } });
						break;

					case ScriptFieldType::Material:
						if (node.is_string())
							field.SetValue<Material>(sComp.instance, Material{ Uuid{ std::stoull(node.get_string()) } });
						break;
					default:
						LOG_WARNING("Unsupported script field type for deserialization: {}", static_cast<uint32_t>(field.type));
						break;
					}
				}
			}
		}


		// set parent-child relationships
		for (const auto& [childId, parentId] : parentMap)
		{
			Entity childEntity = scene.GetEntity(childId);
			Entity parentEntity = scene.GetEntity(parentId);

			auto& childHC = childEntity.GetComponent<HierarchyComponent>();
			childHC.parent = static_cast<entt::entity>(parentEntity.GetHandle());

			auto& parentHC = parentEntity.GetComponent<HierarchyComponent>();
			parentHC.children.push_back(static_cast<entt::entity>(childEntity.GetHandle()));
		}

		return scene;
	}
}