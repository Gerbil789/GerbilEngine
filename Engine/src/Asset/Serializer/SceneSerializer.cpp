#include "enginepch.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Script/Script.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Resources.h"
#include "Engine/Scene/Components.h"
#include <glaze/glaze.hpp>
#include <fstream>

template <>
struct glz::meta<glm::vec2> { static constexpr auto value = array(&glm::vec2::x, &glm::vec2::y); };

template <>
struct glz::meta<glm::vec3> { static constexpr auto value = array(&glm::vec3::x, &glm::vec3::y, &glm::vec3::z); };

template <>
struct glz::meta<glm::vec4> { static constexpr auto value = array(&glm::vec4::x, &glm::vec4::y, &glm::vec4::z, &glm::vec4::w); };

namespace Engine
{
	struct TransformJSON
	{
		glm::vec3 Position{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };
		std::optional<uint64_t> Parent;
	};

	struct MeshComponentJSON
	{
		uint64_t Mesh = 0;
		std::vector<uint64_t> Materials;
	};

	struct ColliderComponentJSON
	{
		uint64_t Mesh = 0;
		uint32_t Type = 0;
		bool IsTrigger = false;
	};

	struct CameraPerspectiveJSON
	{
		float FOV = 0.0f, Near = 0.0f, Far = 0.0f;
	};

	struct CameraOrthographicJSON
	{
		float Size = 0.0f, Near = 0.0f, Far = 0.0f;
	};

	struct CameraComponentJSON
	{
		uint32_t Projection = 0;
		float AspectRatio = 1.0f;
		CameraPerspectiveJSON Perspective;
		CameraOrthographicJSON Orthographic;
		uint32_t Background = 0;
		glm::vec4 ClearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		bool primary = false;
	};

	struct LightComponentJSON
	{
		uint32_t Type = 0;
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float Range = 1.0f;
		float Angle = 1.0f;
	};

	struct ScriptComponentJSON
	{
		uint32_t Script;
		std::map<std::string, glz::generic> Fields;
	};

	struct EntityJSON
	{
		uint64_t ID = 0;
		bool Enabled = true;
		std::optional<std::string> Name;
		std::optional<TransformJSON> Transform;
		std::optional<MeshComponentJSON> MeshComponent;
		std::optional<ColliderComponentJSON> ColliderComponent;
		std::optional<CameraComponentJSON> CameraComponent;
		std::optional<LightComponentJSON> LightComponent;
		std::optional<ScriptComponentJSON> ScriptComponent;
	};
}

template <>
struct glz::meta<Engine::EntityJSON>
{
	using T = Engine::EntityJSON;
	static constexpr auto value = object(
		"ID", &T::ID,
		"Enabled", &T::Enabled,
		"Name", &T::Name,
		"Transform", &T::Transform,
		"MeshComponent", &T::MeshComponent,
		"ColliderComponent", &T::ColliderComponent,
		"CameraComponent", &T::CameraComponent,
		"LightComponent", &T::LightComponent,
		"ScriptComponent", &T::ScriptComponent
	);
};

namespace Engine
{
	namespace
	{
		std::string buffer;
	}

	void SceneSerializer::Serialize(Uuid id, const std::filesystem::path& path)
	{
		auto& scene = AssetManager::GetAsset<Scene>(id);

		if (path.extension() != ".json" && path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' or '.json' extension in save path: '{}'", path);
			return;
		}

		entt::registry& registry = scene.GetRegistry();
		auto view = registry.view<IdentityComponent>();

		std::vector<EntityJSON> sceneData;
		sceneData.reserve(view.size());

		for (entt::entity entity : view)
		{
			EntityJSON eJson;

			//DisabledTag
			if (registry.any_of<DisabledTag>(entity))
			{
				eJson.Enabled = false;
			}

			// Identity
			{
				const auto& identity = registry.get<IdentityComponent>(entity);
				eJson.ID = identity.id;
			}

			// Name
			{
				eJson.Name = registry.get<NameComponent>(entity).name;
			}

			// Transform
			{
				auto& tc = registry.get<TransformComponent>(entity);
				TransformJSON tJson{ tc.position, tc.rotation, tc.scale };

				auto* hc = registry.try_get<HierarchyComponent>(entity);
				if (hc && hc->parent != entt::null)
				{
					auto& parentIdentity = registry.get<IdentityComponent>(hc->parent);
					tJson.Parent = parentIdentity.id;
				}

				eJson.Transform = tJson;
			}



			// Mesh
			if (registry.any_of<MeshComponent>(entity))
			{
				const auto& m = registry.get<MeshComponent>(entity);
				MeshComponentJSON mJson{ m.meshId };
				mJson.Materials.reserve(m.materials.size());
				for (Uuid material : m.materials) mJson.Materials.push_back(static_cast<uint64_t>(material));
				eJson.MeshComponent = mJson;
			}

			// Collider
			if (registry.any_of<ColliderComponent>(entity))
			{
				const auto& c = registry.get<ColliderComponent>(entity);
				eJson.ColliderComponent = ColliderComponentJSON{ c.collisionMeshId, static_cast<uint32_t>(c.type), c.isTrigger };
			}

			// Camera
			if (registry.any_of<CameraComponent>(entity))
			{
				const auto& c = registry.get<CameraComponent>(entity);
				Camera* cam = c.camera;
				CameraComponentJSON cJson;
				cJson.Projection = static_cast<uint32_t>(cam->GetProjection());
				cJson.AspectRatio = cam->GetAspectRatio();

				cJson.Perspective = { cam->GetPerspectiveFOV(), cam->GetPerspectiveNear(), cam->GetPerspectiveFar() };
				cJson.Orthographic = { cam->GetOrthoSize(), cam->GetOrthoNear(), cam->GetOrthoFar() };

				cJson.Background = static_cast<uint32_t>(cam->GetBackground());
				cJson.ClearColor = cam->GetClearColor();
				cJson.primary = c.primary;
				eJson.CameraComponent = cJson;
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
				sJson.Script = component->id;

				const auto& desc = Engine::ScriptRegistry::GetDescriptor(component->id);
				auto* instance = component->instance;
				auto& fields = sJson.Fields;

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
						dst = std::to_string(static_cast<uint64_t>(field.GetValue<Texture2DHandle>(instance).id));
						break;
					case ScriptFieldType::AudioClip:
						dst = std::to_string(static_cast<uint64_t>(field.GetValue<AudioClipHandle>(instance).id));
						break;
					case ScriptFieldType::Mesh:
						dst = std::to_string(static_cast<uint64_t>(field.GetValue<MeshHandle>(instance).id));
						break;
					case ScriptFieldType::Material:
						dst = std::to_string(static_cast<uint64_t>(field.GetValue<MaterialHandle>(instance).id));
						break;

					default:
						LOG_WARNING("Unsupported script field type for serialization: {}",
							static_cast<uint32_t>(field.type));
						break;
					}
				}

				eJson.ScriptComponent = std::move(sJson);
			}

			sceneData.push_back(std::move(eJson));
		}


		if (auto ec = glz::write_file_json<glz::opts{ .prettify = true }>(sceneData, path.string(), buffer))
		{
			LOG_ERROR("Failed to save scene file '{}': {}", path.string(), glz::format_error(ec));
		}
	}


	std::optional<Scene> SceneSerializer::Deserialize(const std::filesystem::path& path)
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

		Scene scene;
		scene.SetEnvironmentTexture(RESOURCES::TEXTURE::HDR); //TODO: store in scene file

		entt::registry& registry = scene.GetRegistry();

		for (const EntityJSON& eJson : json)
		{
			Uuid id = Uuid{ eJson.ID };

			if(!id)
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
			if (!eJson.Enabled)
			{
				registry.emplace<DisabledTag>(handle);
			}

			// name
			if (!eJson.Name.has_value()) throw std::runtime_error("Entity has no name: " + std::to_string(static_cast<uint64_t>(id)));

			auto& nc = registry.emplace<NameComponent>(handle);
			nc.name = eJson.Name.value();

			// transform
			if (!eJson.Transform.has_value()) throw std::runtime_error("Entity has no transform: " + std::to_string(static_cast<uint64_t>(id)));

			auto& tc = registry.emplace<TransformComponent>(handle);
			const auto& tJson = eJson.Transform.value();
			tc.position = tJson.Position;
			tc.rotation = tJson.Rotation;
			tc.scale = tJson.Scale;

			registry.emplace<DirtyTag>(handle);
			registry.emplace<WorldTransformComponent>(handle);
			registry.emplace<HierarchyComponent>(handle);

			if (tJson.Parent.has_value())
			{
				Uuid parentId{ tJson.Parent.value() };
				parentMap[id] = parentId;
			}
			else
			{
				scene.m_RootEntities.push_back(handle);
			}

			// mesh
			if (eJson.MeshComponent.has_value())
			{
				auto& mComp = registry.emplace<MeshComponent>(handle);
				const auto& mJson = eJson.MeshComponent.value();
				mComp.meshId = Uuid{ mJson.Mesh };

				if (!AssetManager::Exists(mComp.meshId))
				{
					mComp.meshId = RESOURCES::MESH::EMPTY;
				}

				mComp.materials.reserve(mJson.Materials.size());

				for (auto rawId : mJson.Materials)
				{
					Engine::Uuid materialId{ static_cast<uint64_t>(rawId) };
					mComp.materials.push_back(Uuid{ materialId });
				}
			}

			// collider
			if (eJson.ColliderComponent.has_value())
			{
				auto& cComp = registry.emplace<ColliderComponent>(handle);
				const auto& cJson = eJson.ColliderComponent.value();
				cComp.collisionMeshId = Uuid{ cJson.Mesh };
				cComp.type = static_cast<BodyType>(cJson.Type);
				cComp.isTrigger = cJson.IsTrigger;
			}

			// Camera
			if (eJson.CameraComponent.has_value())
			{
				auto& cComp = registry.emplace<CameraComponent>(handle);
				const auto& cJson = eJson.CameraComponent.value();

				std::unique_ptr<Camera> camera = std::make_unique<Camera>();
				camera->SetProjection(static_cast<Camera::Projection>(cJson.Projection));
				camera->SetAspectRatio(cJson.AspectRatio);

				camera->SetPerspectiveFOV(cJson.Perspective.FOV);
				camera->SetPerspectiveNear(cJson.Perspective.Near);
				camera->SetPerspectiveFar(cJson.Perspective.Far);

				camera->SetOrthoSize(cJson.Orthographic.Size);
				camera->SetOrthoNear(cJson.Orthographic.Near);
				camera->SetOrthoFar(cJson.Orthographic.Far);

				camera->SetBackground(static_cast<Camera::Background>(cJson.Background));
				camera->SetClearColor(cJson.ClearColor);

				cComp.primary = cJson.primary;

				cComp.camera = camera.release(); // TODO: manage memory lifecycle
			}

			// light
			if (eJson.LightComponent.has_value())
			{
				auto& lComp = registry.emplace<LightComponent>(handle);
				const auto& lJson = eJson.LightComponent.value();
				lComp.type = static_cast<LightType>(lJson.Type);
				lComp.color = lJson.Color;
				lComp.intensity = lJson.Intensity;
				lComp.range = lJson.Range;
				lComp.angle = lJson.Angle;
			}

			// script
			if (eJson.ScriptComponent.has_value())
			{
				auto& sComp = registry.emplace<ScriptComponent>(handle);
				const auto& sJson = eJson.ScriptComponent.value();

				const Engine::ScriptDescriptor& desc = Engine::ScriptRegistry::GetDescriptor(sJson.Script);

				sComp.id = sJson.Script;
				sComp.instance = desc.factory();
				sComp.instance->m_Entity = Entity{ handle, &scene }; //TODO: is scene valid after this functin?
				sComp.instance->OnCreate();

				for (const auto& field : desc.fields)
				{
					if (sJson.Fields.find(field.name) == sJson.Fields.end()) continue;

					const auto& node = sJson.Fields.at(field.name);

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
							field.SetValue<Texture2DHandle>(sComp.instance, Texture2DHandle{ .id = Uuid{ std::stoull(node.get_string()) } });
						break;

					case ScriptFieldType::AudioClip:
						if (node.is_string())
							field.SetValue<AudioClipHandle>(sComp.instance, AudioClipHandle{ .id = Uuid{ std::stoull(node.get_string()) } });
						break;

					case ScriptFieldType::Mesh:
						if (node.is_string())
							field.SetValue<MeshHandle>(sComp.instance, MeshHandle{ .id = Uuid{ std::stoull(node.get_string()) } });
						break;

					case ScriptFieldType::Material:
						if (node.is_string())
							field.SetValue<MaterialHandle>(sComp.instance, MaterialHandle{ .id = Uuid{ std::stoull(node.get_string()) } });
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