#include "enginepch.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Utility/Yaml.h"

namespace Engine
{
	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		Engine::Yaml::Map entityMap(out);

		// Identity
		const auto& identityComponent = entity.GetComponent<IdentityComponent>();

		Engine::Yaml::Write(out, "ID", identityComponent.id);
		Engine::Yaml::Write(out, "Enabled", identityComponent.enabled);

		// Name
		Engine::Yaml::Write(out, "Name", entity.GetComponent<NameComponent>().name);

		// Transform
		const auto& transformComponent = entity.GetComponent<TransformComponent>();
		{
			Engine::Yaml::Map transformMap(out, "Transform");

			Engine::Yaml::Write(out, "Position", transformComponent.position);
			Engine::Yaml::Write(out, "Rotation", transformComponent.rotation);
			Engine::Yaml::Write(out, "Scale", transformComponent.scale);
		}

		// Mesh
		if (entity.HasComponent<MeshComponent>())
		{
			const auto& component = entity.GetComponent<MeshComponent>();
			const auto& mesh = component.mesh;
			const auto& material = component.material;

			Engine::Yaml::Map meshMap(out, "MeshComponent");

			if (mesh) Engine::Yaml::Write(out, "Mesh", mesh->id);
			if (material) Engine::Yaml::Write(out, "Material", material->id);
		}

		// Camera
		if (entity.HasComponent<CameraComponent>())
		{
			const auto& component = entity.GetComponent<CameraComponent>();
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
		if (entity.HasComponent<LightComponent>())
		{
			const auto& component = entity.GetComponent<LightComponent>();

			Engine::Yaml::Map cameraMap(out, "LightComponent");
			Engine::Yaml::Write(out, "Type", static_cast<uint32_t>(component.type)); //TODO: serialize string
			Engine::Yaml::Write(out, "Color", component.color);
			Engine::Yaml::Write(out, "Intensity", component.intensity);
			Engine::Yaml::Write(out, "Range", component.range);
			Engine::Yaml::Write(out, "Angle", component.angle);
		}

		// AudioSource
		if (entity.HasComponent<AudioSourceComponent>())
		{
			const auto& component = entity.GetComponent<AudioSourceComponent>();

			Engine::Yaml::Map cameraMap(out, "AudioSourceComponent");
			if (component.clip) Engine::Yaml::Write(out, "AudioClip", component.clip->id);
			Engine::Yaml::Write(out, "Volume", component.volume);
			Engine::Yaml::Write(out, "Loop", component.loop);
			Engine::Yaml::Write(out, "PlayOnAwake", component.playOnAwake);
		}

		// AudioListener
		if (entity.HasComponent<AudioListenerComponent>())
		{
			//const auto& component = entity.GetComponent<AudioListenerComponent>();

			Engine::Yaml::Map cameraMap(out, "AudioListenerComponent");
			//TODO...
		}

		// Script
		if (entity.HasComponent<ScriptComponent>())
		{
			//const auto& component = entity.GetComponent<ScriptComponent>();

			Engine::Yaml::Map cameraMap(out, "ScriptComponent");
			//TODO...
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

		//std::string sceneName = path.stem().string();
		auto scene = new Scene();

		for (auto entityNode : root)
		{
			Entity entity = scene->CreateEntity();

			// Identity
			uint64_t uuid = entityNode["ID"].as<uint64_t>();
			bool enabled = entityNode["Enabled"].as<bool>(true);
			auto& identity = entity.GetComponent<IdentityComponent>();
			identity.id = uuid;
			identity.enabled = enabled;

			// Name
			if (entityNode["Name"])
			{
				auto& nameComp = entity.GetComponent<NameComponent>();
				nameComp.name = entityNode["Name"].as<std::string>();
			}

			// Transform
			if (entityNode["Transform"])
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				auto transformNode = entityNode["Transform"];

				Engine::Yaml::Read(transformNode["Position"], transform.position);
				Engine::Yaml::Read(transformNode["Rotation"], transform.rotation);
				Engine::Yaml::Read(transformNode["Scale"], transform.scale);
			}

			// Mesh
			if (auto meshNode = entityNode["MeshComponent"]; meshNode)
			{
				auto& component = entity.AddComponent<MeshComponent>();
				uint64_t id;
				if (Engine::Yaml::Read<uint64_t>(meshNode, "Mesh", id))
				{
					component.mesh = AssetManager::GetAsset<Mesh>(id);
				}

				if (Engine::Yaml::Read<uint64_t>(meshNode, "Material", id))
				{
					component.material = AssetManager::GetAsset<Material>(id);
				}
			}

			// Camera
			if(auto camNode = entityNode["CameraComponent"]; camNode)
			{
				auto& component = entity.AddComponent<CameraComponent>();
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
				auto& component = entity.AddComponent<LightComponent>();

				if (uint32_t type; Engine::Yaml::Read(lightNode, "Type", type))
				{
					component.type = static_cast<LightType>(type);
				}

				Engine::Yaml::Read(lightNode["Color"], component.color);
				Engine::Yaml::Read(lightNode, "Intensity", component.intensity);
				Engine::Yaml::Read(lightNode, "Range", component.range);
				Engine::Yaml::Read(lightNode, "Angle", component.angle);
			}

			// Audio Source
			if (auto audioSourceNode = entityNode["AudioSourceComponent"]; audioSourceNode)
			{
				auto& component = entity.AddComponent<AudioSourceComponent>();

				if (uint64_t id; Engine::Yaml::Read<uint64_t>(audioSourceNode, "AudioClip", id))
				{
					component.clip = AssetManager::GetAsset<AudioClip>(id);
				}

				Engine::Yaml::Read(audioSourceNode, "Volume", component.volume);
				Engine::Yaml::Read(audioSourceNode, "Loop", component.loop);
				Engine::Yaml::Read(audioSourceNode, "PlayOnAwake", component.playOnAwake);
			}

			// Audio Listener
			{

			}

			// Script
			{

			}
		}

		return scene;
	}
}