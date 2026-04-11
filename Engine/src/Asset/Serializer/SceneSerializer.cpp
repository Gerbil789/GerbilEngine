#include "enginepch.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Script/Script.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Utility/Yaml.h"

namespace Engine
{
	ScriptRegistry* m_Registry = nullptr;

	void SceneSerializer::Initialize(ScriptRegistry& registry)
	{
		m_Registry = &registry;
	}


	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		Engine::Yaml::Map entityMap(out);

		// Identity
		const auto& identityComponent = entity.Get<IdentityComponent>();

		Engine::Yaml::Write(out, "ID", identityComponent.id);
		Engine::Yaml::Write(out, "Enabled", identityComponent.enabled);

		// Name
		Engine::Yaml::Write(out, "Name", entity.Get<NameComponent>().name);

		// Transform
		const auto& transformComponent = entity.Get<TransformComponent>();
		{
			Engine::Yaml::Map transformMap(out, "Transform");

			Engine::Yaml::Write(out, "Position", transformComponent.position);
			Engine::Yaml::Write(out, "Rotation", transformComponent.rotation);
			Engine::Yaml::Write(out, "Scale", transformComponent.scale);
		}

		// Mesh
		if (entity.Has<MeshComponent>())
		{
			const auto& component = entity.Get<MeshComponent>();
			const auto& mesh = component.mesh;
			const auto& materials = component.materials;

			Engine::Yaml::Map meshMap(out, "MeshComponent");

			if (mesh)
			{
				Engine::Yaml::Write(out, "Mesh", mesh->id);

				{
					Engine::Yaml::Seq materialSeq(out, "Materials", true);
					for (const auto& material : materials)
					{
						if (material) 
						{
							out << static_cast<uint64_t>(material->id);
						}
						
					}
				}
			}
		}

		// Camera
		if (entity.Has<CameraComponent>())
		{
			const auto& component = entity.Get<CameraComponent>();
			Camera* camera = component.camera;

			Engine::Yaml::Map cameraMap(out, "CameraComponent");
			Engine::Yaml::Write(out, "Projection", static_cast<uint32_t>(camera->GetProjection())); //TODO: serialize string
			Engine::Yaml::Write(out, "AspectRatio", camera->GetAspectRatio());

			{
				Engine::Yaml::Map perspectiveMap(out, "Perspective");

				const auto& perspective = camera->Perspective();
				Engine::Yaml::Write(out, "FOV", perspective.fov);
				Engine::Yaml::Write(out, "Near", perspective.near);
				Engine::Yaml::Write(out, "Far", perspective.far);
			}

			{
				Engine::Yaml::Map orthographicMap(out, "Orthographic");

				const auto& orthographic = camera->Orthographic();
				Engine::Yaml::Write(out, "Size", orthographic.size);
				Engine::Yaml::Write(out, "Near", orthographic.near);
				Engine::Yaml::Write(out, "Far", orthographic.far);
			}

			Engine::Yaml::Write(out, "Background", static_cast<uint32_t>(camera->GetBackground())); //TODO: serialize string
			Engine::Yaml::Write(out, "ClearColor", camera->GetClearColor());

			//TODO: serialize environment (skybox/sphere)
		}


		// Light
		if (entity.Has<LightComponent>())
		{
			const auto& component = entity.Get<LightComponent>();

			Engine::Yaml::Map cameraMap(out, "LightComponent");
			Engine::Yaml::Write(out, "Type", static_cast<uint32_t>(component.type)); //TODO: serialize string
			Engine::Yaml::Write(out, "Color", component.color);
			Engine::Yaml::Write(out, "Intensity", component.intensity);
			Engine::Yaml::Write(out, "Range", component.range);
			Engine::Yaml::Write(out, "Angle", component.angle);
		}

		// Script
		if (entity.Has<ScriptComponent>())
		{
			const auto& component = entity.Get<ScriptComponent>();

			auto& desc = m_Registry->GetDescriptor(component.id);

			const auto& fields = desc.fields;

			std::byte* base = reinterpret_cast<std::byte*>(component.instance);

			Engine::Yaml::Map cameraMap(out, "ScriptComponent");

			Engine::Yaml::Write(out, "Script", desc.name);

			for (const auto& field : fields)
			{
				void* fieldPtr = base + field.offset;

				switch (field.type)
				{
				case ScriptFieldType::Bool:
				{
					auto& value = *reinterpret_cast<bool*>(fieldPtr);
					Engine::Yaml::Write(out, field.name, value);
					break;
				}
				case ScriptFieldType::Int:
				{
					auto& value = *reinterpret_cast<int*>(fieldPtr);
					Engine::Yaml::Write(out, field.name, value);
					break;
				}
				case ScriptFieldType::Float:
				{
					auto& value = *reinterpret_cast<float*>(fieldPtr);
					Engine::Yaml::Write(out, field.name, value);
					break;
				}
				case ScriptFieldType::Texture:
				{
					auto& texture = *reinterpret_cast<Texture2D**>(fieldPtr);
					Engine::Yaml::Write(out, field.name, texture ? static_cast<uint64_t>(texture->id) : 0);
					break;

				}
				case ScriptFieldType::AudioClip:
				{
					auto& clip = *reinterpret_cast<AudioClip**>(fieldPtr);
					Engine::Yaml::Write(out, field.name, clip ? static_cast<uint64_t>(clip->id) : 0);
					break;
				}
				case ScriptFieldType::Mesh:
				{
					auto& mesh = *reinterpret_cast<Mesh**>(fieldPtr);
					Engine::Yaml::Write(out, field.name, mesh ? static_cast<uint64_t>(mesh->id) : 0);
					break;
				}

				case ScriptFieldType::Material:
				{
					auto& material = *reinterpret_cast<Material**>(fieldPtr);
					Engine::Yaml::Write(out, field.name, material ? static_cast<uint64_t>(material->id) : 0);
					break;
				}
				}
			}
		}
	}

	void SceneSerializer::Serialize(Scene* scene, const std::filesystem::path& path)
	{
		if (!scene)
		{
			LOG_ERROR("Scene is null, cannot serialize");
			return;
		}

		if (path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' extension, got '{}'", path.extension().string());
			return;
		}

		YAML::Emitter out;

		{
			Engine::Yaml::Seq seq(out);
			const auto& entities = scene->GetEntities(true);
			for (const Entity& entity : entities)
			{
				SerializeEntity(out, entity);
			}
		}

		std::ofstream fout(path);
		fout << out.c_str();
	}


	Scene* SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		if (path.extension() != ".scene")
		{
			LOG_ERROR("Expected '.scene' extension, got '{}'", path.extension().string());
			return nullptr;
		}

		YAML::Node root;
		try
		{
			root = YAML::LoadFile(path.string());
		}
		catch (const YAML::ParserException& e)
		{
			LOG_ERROR("Failed to load scene file '{}': {}", path, e.what());
			return nullptr;
		}

		auto scene = new Scene();

		for (auto entityNode : root)
		{
			Entity entity = scene->CreateEntity();

			// Identity
			uint64_t uuid = entityNode["ID"].as<uint64_t>();
			bool enabled = entityNode["Enabled"].as<bool>(true);
			auto& identity = entity.Get<IdentityComponent>();
			identity.id = uuid;
			identity.enabled = enabled;

			// Name
			if (entityNode["Name"])
			{
				auto& nameComp = entity.Get<NameComponent>();
				nameComp.name = entityNode["Name"].as<std::string>();
			}

			// Transform
			if (entityNode["Transform"])
			{
				auto& transform = entity.Get<TransformComponent>();
				auto transformNode = entityNode["Transform"];

				Engine::Yaml::Read(transformNode["Position"], transform.position);
				Engine::Yaml::Read(transformNode["Rotation"], transform.rotation);
				Engine::Yaml::Read(transformNode["Scale"], transform.scale);
			}

			// Mesh
			if (auto meshNode = entityNode["MeshComponent"]; meshNode)
			{
				auto& component = entity.Add<MeshComponent>();

				if (uint64_t id; Engine::Yaml::Read<uint64_t>(meshNode, "Mesh", id))
				{
					component.mesh = Engine::g_AssetManager->GetAsset<Mesh>(id);
				}

				// Materials
				auto materialsNode = meshNode["Materials"];
				if (materialsNode)
				{
					int i = 0;
					for (const auto& materialNode : materialsNode)
					{
						uint64_t materialId = materialNode.as<uint64_t>();
						Material* material = Engine::g_AssetManager->GetAsset<Material>(materialId);
						component.SetMaterial(i++, material);
					}
				}


			}

			// Camera
			if(auto camNode = entityNode["CameraComponent"]; camNode)
			{
				auto& component = entity.Add<CameraComponent>();
				std::unique_ptr<Camera> camera = std::make_unique<Camera>();

				if(uint32_t projection; Engine::Yaml::Read(camNode, "Projection", projection))
				{
					camera->SetProjection(static_cast<Camera::Projection>(projection));
				}

				if (float ratio; Engine::Yaml::Read(camNode, "AspectRatio", ratio))
				{
					camera->SetAspectRatio(ratio);
				}

				auto perspectiveNode = camNode["Perspective"];
				auto& perspective = camera->Perspective();

				Engine::Yaml::Read(perspectiveNode, "FOV", perspective.fov);
				Engine::Yaml::Read(perspectiveNode, "Near", perspective.near);
				Engine::Yaml::Read(perspectiveNode, "Far", perspective.far);

				auto orthographicNode = camNode["Orthographic"];
				auto& orthographic = camera->Orthographic();

				Engine::Yaml::Read(orthographicNode, "FOV", orthographic.size);
				Engine::Yaml::Read(orthographicNode, "Near", orthographic.near);
				Engine::Yaml::Read(orthographicNode, "Far", orthographic.far);

				if (uint32_t background; Engine::Yaml::Read(camNode, "Background", background))
				{
					camera->SetBackground(static_cast<Camera::Background>(background));
				}

				if (glm::vec4 color; Engine::Yaml::Read(camNode["ClearColor"], color))
				{
					camera->SetClearColor(color);
				}

				component.camera = camera.release(); // pass ownership to the component, TODO: possible memory leak later :p
			}

			// Light
			if (auto lightNode = entityNode["LightComponent"]; lightNode)
			{
				auto& component = entity.Add<LightComponent>();

				if (uint32_t type; Engine::Yaml::Read(lightNode, "Type", type))
				{
					component.type = static_cast<LightType>(type);
				}

				Engine::Yaml::Read(lightNode["Color"], component.color);
				Engine::Yaml::Read(lightNode, "Intensity", component.intensity);
				Engine::Yaml::Read(lightNode, "Range", component.range);
				Engine::Yaml::Read(lightNode, "Angle", component.angle);
			}

			// Script
			if (auto scriptNode = entityNode["ScriptComponent"]; scriptNode)
			{
				auto& component = entity.Add<ScriptComponent>();

				std::string scriptName = scriptNode["Script"].as<std::string>();
				auto desc = m_Registry->GetDescriptor(scriptName);

				component.id = desc.name;
				component.instance = desc.factory();
				component.instance->Self = entity;
				component.instance->OnCreate();

				std::byte* base = reinterpret_cast<std::byte*>(component.instance);

				for (const auto& field : desc.fields)
				{
					void* fieldPtr = base + field.offset;
					switch (field.type)
					{
					case ScriptFieldType::Bool:
					{
						bool value = false;
						Engine::Yaml::Read(scriptNode, field.name, value);
						*reinterpret_cast<bool*>(fieldPtr) = value;
						break;
					}
					case ScriptFieldType::Int:
					{
						int value = 0;
						Engine::Yaml::Read(scriptNode, field.name, value);
						*reinterpret_cast<int*>(fieldPtr) = value;
						break;
					}
					case ScriptFieldType::Float:
					{
						float value = 0.0f;
						Engine::Yaml::Read(scriptNode, field.name, value);
						*reinterpret_cast<float*>(fieldPtr) = value;
						break;
					}
					case ScriptFieldType::Texture:
					{
						uint64_t textureId = 0;
						Engine::Yaml::Read(scriptNode, field.name, textureId);
						Texture2D* texture = Engine::g_AssetManager->GetAsset<Texture2D>(textureId);
						*reinterpret_cast<Texture2D**>(fieldPtr) = texture;
						break;
					}
					case ScriptFieldType::AudioClip:
					{
						uint64_t clipId = 0;
						Engine::Yaml::Read(scriptNode, field.name, clipId);
						AudioClip* clip = Engine::g_AssetManager->GetAsset<AudioClip>(clipId);
						*reinterpret_cast<AudioClip**>(fieldPtr) = clip;
						break;
					}
					case ScriptFieldType::Mesh:
					{
						uint64_t meshId = 0;
						Engine::Yaml::Read(scriptNode, field.name, meshId);
						Mesh* mesh = Engine::g_AssetManager->GetAsset<Mesh>(meshId);
						*reinterpret_cast<Mesh**>(fieldPtr) = mesh;
						break;
					}
					case ScriptFieldType::Material:
					{
						uint64_t materialId = 0;
						Engine::Yaml::Read(scriptNode, field.name, materialId);
						Material* material = Engine::g_AssetManager->GetAsset<Material>(materialId);
						*reinterpret_cast<Material**>(fieldPtr) = material;
						break;
					}
					}
				}
			}
		}

		return scene;
	}
}