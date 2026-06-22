#include "enginepch.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Script/Script.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Core/Resources.h"
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
	void SceneSerializer::Serialize(Scene& scene, const std::filesystem::path& path)
	{
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

			// Identity
			{
				const auto& identity = registry.get<IdentityComponent>(entity);
				eJson.ID = identity.id;
				eJson.Enabled = identity.enabled;
			}

			// Name
			{
				eJson.Name = registry.get<NameComponent>(entity).name;
			}

			// Transform
			{
				const auto& t = registry.get<TransformComponent>(entity);
				TransformJSON tJson{ t.position, t.rotation, t.scale };

				if (t.parent != entt::null)
				{
					tJson.Parent = registry.get<IdentityComponent>(t.parent).id;
				}
				eJson.Transform = tJson;
			}

			// Mesh
			if (registry.any_of<MeshComponent>(entity))
			{
				const auto& m = registry.get<MeshComponent>(entity);
				MeshComponentJSON mJson{ m.meshId };
				mJson.Materials.reserve(m.materials.size());
				for (Uuid id : m.materials) mJson.Materials.push_back((uint64_t)id);
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

		std::string buffer{};
		if (auto ec = glz::write_file_json(sceneData, path.string(), buffer))
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

		AssetRegistry& assetRegistry = AssetManager::GetAssetRegistry();

		std::vector<EntityJSON> sceneData;
		std::string buffer;

		if (auto ec = glz::read_file_json(sceneData, path.string(), buffer))
		{
			LOG_ERROR("Failed to load scene file '{}': {}", path.string(), glz::format_error(ec, buffer));
			return std::nullopt;
		}

		Scene scene;
		entt::registry& registry = scene.GetRegistry();
		std::unordered_map<uint64_t, entt::entity> entityMap;
		std::unordered_map<uint64_t, uint64_t> entityParentMap;

		scene.SetEnvironmentTexture(RESOURCES::TEXTURE::HDR); //TODO: store in scene file

		for (const auto& eJson : sceneData)
		{
			Uuid entityId{ eJson.ID };
			std::string name = eJson.Name.value_or("Entity");

			entt::entity entity = scene.CreateEntity(name, entityId);

			// Identity
			auto& identity = registry.get<IdentityComponent>(entity);
			identity.enabled = eJson.Enabled;
			entityMap[entityId] = entity;

			// Transform
			if (eJson.Transform.has_value())
			{
				auto& tComp = registry.get<TransformComponent>(entity);
				const auto& tJson = eJson.Transform.value();
				tComp.position = tJson.Position;
				tComp.rotation = tJson.Rotation;
				tComp.scale = tJson.Scale;

				if (tJson.Parent.has_value())
					entityParentMap[eJson.ID] = tJson.Parent.value();
			}

			// Mesh
			if (eJson.MeshComponent.has_value())
			{
				auto& mComp = registry.emplace<MeshComponent>(entity);
				const auto& mJson = eJson.MeshComponent.value();
				mComp.meshId = Uuid{ mJson.Mesh };

				if(!assetRegistry.GetRecord(mComp.meshId))
				{
					mComp.meshId = RESOURCES::MESH::EMPTY;
				}

				mComp.materials.reserve(mJson.Materials.size());

				for (auto rawId : mJson.Materials)
				{
					Engine::Uuid id{ static_cast<uint64_t>(rawId) };
					if (assetRegistry.GetRecord(id))
					{
						mComp.materials.push_back(Uuid{ id });
					}
					else
					{
						mComp.materials.push_back(RESOURCES::MATERIAL::PINK);
					}
				}
			}

			// Collider
			if (eJson.ColliderComponent.has_value())
			{
				auto& cComp = registry.emplace<ColliderComponent>(entity);
				const auto& cJson = eJson.ColliderComponent.value();
				cComp.collisionMeshId = Uuid{ cJson.Mesh };
				cComp.type = static_cast<BodyType>(cJson.Type);
				cComp.isTrigger = cJson.IsTrigger;
			}

			// Camera
			if (eJson.CameraComponent.has_value())
			{
				auto& cComp = registry.emplace<CameraComponent>(entity);
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

			// Light
			if (eJson.LightComponent.has_value())
			{
				auto& lComp = registry.emplace<LightComponent>(entity);
				const auto& lJson = eJson.LightComponent.value();
				lComp.type = static_cast<LightType>(lJson.Type);
				lComp.color = lJson.Color;
				lComp.intensity = lJson.Intensity;
				lComp.range = lJson.Range;
				lComp.angle = lJson.Angle;
			}

			// Script
			if (eJson.ScriptComponent.has_value())
			{
				auto& sComp = registry.emplace<ScriptComponent>(entity);
				const auto& sJson = eJson.ScriptComponent.value();

				const Engine::ScriptDescriptor& desc = Engine::ScriptRegistry::GetDescriptor(sJson.Script);

				sComp.id = sJson.Script;
				sComp.instance = desc.factory();
				sComp.instance->m_Entity = entity;
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
						LOG_WARNING("Unsupported script field type for deserialization: {}",static_cast<uint32_t>(field.type));
						break;
					}
				}
			}
		}

		// Rebuild Parent-Child hierarchy
		for (const auto& [childID, parentID] : entityParentMap)
		{
			if (parentID != 0 && entityMap.find(parentID) != entityMap.end())
			{
				entt::entity child = scene.GetEntity(childID);
				entt::entity parent = scene.GetEntity(parentID);
				registry.get<Engine::TransformComponent>(child).parent = parent;
			}
		}

		return scene;
	}
}