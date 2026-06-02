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

// =========================================================================
// GLM GLAZE METADATA (Serialize vectors as fast JSON arrays)
// =========================================================================

template <>
struct glz::meta<glm::vec2> { static constexpr auto value = array(&glm::vec2::x, &glm::vec2::y); };

template <>
struct glz::meta<glm::vec3> { static constexpr auto value = array(&glm::vec3::x, &glm::vec3::y, &glm::vec3::z); };

template <>
struct glz::meta<glm::vec4> { static constexpr auto value = array(&glm::vec4::x, &glm::vec4::y, &glm::vec4::z, &glm::vec4::w); };

// =========================================================================
// COMPONENT JSON DTOs
// =========================================================================

namespace Engine
{
	struct TransformJSON {
		glm::vec3 Position{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };
		std::optional<uint64_t> Parent;
	};

	struct MeshComponentJSON {
		uint64_t Mesh = 0;
		std::vector<uint64_t> Materials;
	};

	struct ColliderComponentJSON {
		uint64_t Mesh = 0;
		uint32_t Type = 0;
		bool IsTrigger = false;
	};

	struct CameraPerspectiveJSON {
		float FOV = 0.0f, Near = 0.0f, Far = 0.0f;
	};

	struct CameraOrthographicJSON {
		float Size = 0.0f, Near = 0.0f, Far = 0.0f;
	};

	struct CameraComponentJSON {
		uint32_t Projection = 0;
		float AspectRatio = 1.0f;
		CameraPerspectiveJSON Perspective;
		CameraOrthographicJSON Orthographic;
		uint32_t Background = 0;
		glm::vec4 ClearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
	};

	struct LightComponentJSON {
		uint32_t Type = 0;
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float Range = 1.0f;
		float Angle = 1.0f;
	};

	struct ScriptComponentJSON {
		std::string Script;
		std::map<std::string, glz::generic> Fields; // Dynamic variables
	};

	struct EntityJSON {
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

// =========================================================================
// ENTITY METADATA 
// =========================================================================

template <>
struct glz::meta<Engine::EntityJSON> {
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

// =========================================================================
// SERIALIZER IMPLEMENTATION
// =========================================================================

namespace Engine
{
	void SceneSerializer::Serialize(Scene& scene, const std::filesystem::path& path)
	{
		if (path.extension() != ".json" && path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' or '.json' extension, got '{}'", path.extension().string());
			return;
		}

		std::vector<EntityJSON> sceneData;
		entt::registry& registry = scene.GetRegistry();
		auto view = registry.view<IdentityComponent>();

		for (entt::entity entity : view)
		{
			EntityJSON eJson;

			// Identity
			const auto& identity = registry.get<IdentityComponent>(entity);
			eJson.ID = identity.id;
			eJson.Enabled = identity.enabled;

			// Name
			if (registry.any_of<NameComponent>(entity))
			{
				eJson.Name = registry.get<NameComponent>(entity).name;
			}

			// Transform
			if (registry.any_of<TransformComponent>(entity))
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
				for (auto id : m.materials) mJson.Materials.push_back((uint64_t)id);
				eJson.MeshComponent = mJson;
			}

			// Collider
			if (registry.any_of<ColliderComponent>(entity))
			{
				const auto& c = registry.get<ColliderComponent>(entity);
				eJson.ColliderComponent = ColliderComponentJSON{ c.meshId, static_cast<uint32_t>(c.type), c.isTrigger };
			}

			// Camera
			if (registry.any_of<CameraComponent>(entity))
			{
				Camera* cam = registry.get<CameraComponent>(entity).camera;
				CameraComponentJSON cJson;
				cJson.Projection = static_cast<uint32_t>(cam->GetProjection());
				cJson.AspectRatio = cam->GetAspectRatio();

				cJson.Perspective = { cam->GetPerspectiveFOV(), cam->GetPerspectiveNear(), cam->GetPerspectiveFar() };
				cJson.Orthographic = { cam->GetOrthoSize(), cam->GetOrthoNear(), cam->GetOrthoFar() };

				cJson.Background = static_cast<uint32_t>(cam->GetBackground());
				cJson.ClearColor = cam->GetClearColor();
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
			if (registry.any_of<ScriptComponent>(entity))
			{
				const auto& s = registry.get<ScriptComponent>(entity);
				auto& desc = Engine::g_ScriptRegistry.GetDescriptor(s.id);
				std::byte* base = reinterpret_cast<std::byte*>(s.instance);

				ScriptComponentJSON sJson;
				sJson.Script = desc.name;

				for (const auto& field : desc.fields)
				{
					void* fieldPtr = base + field.offset;
					switch (field.type)
					{
					case ScriptFieldType::Bool:  sJson.Fields[field.name] = *reinterpret_cast<bool*>(fieldPtr); break;
					case ScriptFieldType::Int:   sJson.Fields[field.name] = *reinterpret_cast<int*>(fieldPtr); break;
					case ScriptFieldType::Float: sJson.Fields[field.name] = *reinterpret_cast<float*>(fieldPtr); break;

						// For assets, we store the UUID
					case ScriptFieldType::Texture:
						if (auto* tex = *reinterpret_cast<Texture2D**>(fieldPtr)) sJson.Fields[field.name] = (uint64_t)tex->id; break;
					case ScriptFieldType::AudioClip:
						if (auto* clip = *reinterpret_cast<AudioClip**>(fieldPtr)) sJson.Fields[field.name] = (uint64_t)clip->id; break;
					case ScriptFieldType::Mesh:
						if (auto* mesh = *reinterpret_cast<Mesh**>(fieldPtr)) sJson.Fields[field.name] = (uint64_t)mesh->id; break;
					case ScriptFieldType::Material:
						if (auto* mat = *reinterpret_cast<Material**>(fieldPtr)) sJson.Fields[field.name] = (uint64_t)mat->id; break;
					}
				}
				eJson.ScriptComponent = sJson;
			}

			sceneData.push_back(std::move(eJson));
		}

		std::string buffer;
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

		for (const auto& eJson : sceneData)
		{
			entt::entity entity = scene.CreateEntity();

			// Identity
			auto& identity = registry.get<IdentityComponent>(entity);
			identity.id = eJson.ID;
			identity.enabled = eJson.Enabled;
			entityMap[eJson.ID] = entity;

			// Name
			if (eJson.Name.has_value())
			{
				registry.get<NameComponent>(entity).name = eJson.Name.value();
			}

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
				mComp.meshId = mJson.Mesh;

				if(!assetRegistry.GetRecord(mComp.meshId).IsValid())
				{
					mComp.meshId = RESOURCES::MESH::EMPTY;
				}

				mComp.materials.reserve(mJson.Materials.size());

				for (auto id : mJson.Materials)
				{
					if (assetRegistry.GetRecord(id).IsValid())
					{
						mComp.materials.push_back(Uuid(id));
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
				cComp.meshId = cJson.Mesh;
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

				auto desc = Engine::g_ScriptRegistry.GetDescriptor(sJson.Script);
				sComp.id = desc.name;
				sComp.instance = desc.factory();
				sComp.instance->m_Entity = entity;
				sComp.instance->OnCreate();

				std::byte* base = reinterpret_cast<std::byte*>(sComp.instance);

				for (const auto& field : desc.fields)
				{
					if (sJson.Fields.find(field.name) == sJson.Fields.end()) continue;
					const auto& node = sJson.Fields.at(field.name);
					void* fieldPtr = base + field.offset;

					switch (field.type)
					{
					case ScriptFieldType::Bool:
						if (node.is_boolean()) *reinterpret_cast<bool*>(fieldPtr) = node.get_boolean();
						break;
					case ScriptFieldType::Int:
						if (node.is_number()) *reinterpret_cast<int*>(fieldPtr) = static_cast<int>(static_cast<uint64_t>(node.get_number()));
						break;
					case ScriptFieldType::Float:
						if (node.is_number()) *reinterpret_cast<float*>(fieldPtr) = static_cast<float>(node.get_number());
						break;
					case ScriptFieldType::Texture:
						if (node.is_number()) *reinterpret_cast<Texture2D**>(fieldPtr) = &Engine::AssetManager::GetAsset<Texture2D>(static_cast<uint64_t>(node.get_number()));
						break;
					case ScriptFieldType::AudioClip:
						if (node.is_number()) *reinterpret_cast<AudioClip**>(fieldPtr) = &Engine::AssetManager::GetAsset<AudioClip>(static_cast<uint64_t>(node.get_number()));
						break;
					case ScriptFieldType::Mesh:
						if (node.is_number()) *reinterpret_cast<Mesh**>(fieldPtr) = &Engine::AssetManager::GetAsset<Mesh>(static_cast<uint64_t>(node.get_number()));
						break;
					case ScriptFieldType::Material:
						if (node.is_number()) *reinterpret_cast<Material**>(fieldPtr) = &Engine::AssetManager::GetAsset<Material>(static_cast<uint64_t>(node.get_number()));
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