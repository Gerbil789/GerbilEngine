#include "enginepch.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include <yaml-cpp/yaml.h>

namespace Engine
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////// HELPERS //////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}


	static glm::vec3 ReadVec3(const YAML::Node& node)
	{
		return glm::vec3(
			node[0].as<float>(),
			node[1].as<float>(),
			node[2].as<float>()
		);
	}

	//static glm::vec4 ReadVec4(const YAML::Node& node)
	//{
	//	return glm::vec4(
	//		node[0].as<float>(),
	//		node[1].as<float>(),
	//		node[2].as<float>(),
	//		node[3].as<float>()
	//	);
	//}

	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////// SERIALIZATION ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Entity

		// Identity
		auto& identityComponent = entity.GetComponent<IdentityComponent>();
		auto& id = identityComponent.id;
		auto& enabled = identityComponent.enabled;

		out << YAML::Key << "ID" << YAML::Value << id;
		out << YAML::Key << "Enabled" << YAML::Value << enabled;

		// Name
		auto& nameComponent = entity.GetComponent<NameComponent>();
		out << YAML::Key << "Name" << YAML::Value << nameComponent.name;

		// Transform
		auto& transformComponent = entity.GetComponent<TransformComponent>();

		out << YAML::Key << "Transform";
		out << YAML::BeginMap;

		out << YAML::Key << "Translation" << YAML::Value << transformComponent.position;
		out << YAML::Key << "Rotation" << YAML::Value << transformComponent.rotation;
		out << YAML::Key << "Scale" << YAML::Value << transformComponent.scale;

		out << YAML::EndMap; // Transform

		// Mesh
		if(entity.HasComponent<MeshComponent>())
		{
			auto& meshComponent = entity.GetComponent<MeshComponent>();
			auto& mesh = meshComponent.mesh;
			auto& material = meshComponent.material;

			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;

			if(mesh) { out << YAML::Key << "Mesh" << YAML::Value << mesh->id; }

			if (material) { out << YAML::Key << "Material" << YAML::Value << material->id; }

			out << YAML::EndMap; // MeshComponent
		}

		// Camera
		//if (entity.HasComponent<CameraComponent>())
		//{
		//	auto& cameraComponent = entity.GetComponent<CameraComponent>();
		//	auto& camera = cameraComponent.Camera;
		//}


		// Light
		// ...

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(Scene* scene, const std::filesystem::path& path)
	{
		if(!scene)
		{
			LOG_ERROR("Scene is null, cannot serialize.");
			return;
		}

		if(path.extension() != ".scene")
		{
			LOG_ERROR("SceneSerializer::Serialize - Expected .scene extension, got {0}", path.extension().string());
			return;
		}

		std::string title = path.stem().string();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << title;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		const std::vector<Entity>& entities = scene->GetEntities(true);
		for(auto ent : entities)
		{
			SerializeEntity(out, ent);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////// DESERIALIZATION //////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	Scene* SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		if (path.extension() != ".scene")
		{
			LOG_ERROR("SceneSerializer::Deserialize - Expected .scene extension, got {0}", path.extension().string());
			return nullptr;
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (const YAML::ParserException& e)
		{
			LOG_ERROR("SceneSerializer::Deserialize - Failed to load scene file '{0}': {1}", path.string(), e.what());
			return nullptr;
		}

		if (!data["Scene"])
		{
			LOG_ERROR("SceneSerializer::Deserialize - Missing 'Scene' node in file {0}", path.string());
			return nullptr;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		LOG_INFO("Deserializing scene '{0}'", sceneName);

		auto scene = new Scene();

		// Entities
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entityNode : entities)
			{
				// Identity
				uint64_t uuid = entityNode["ID"].as<uint64_t>();
				bool enabled = entityNode["Enabled"].as<bool>(true);

				Entity entity = scene->CreateEntity(uuid);

				// Identity component
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

					if (transformNode["Translation"])
						transform.position = ReadVec3(transformNode["Translation"]);

					if (transformNode["Rotation"])
						transform.rotation = ReadVec3(transformNode["Rotation"]);

					if (transformNode["Scale"])
						transform.scale = ReadVec3(transformNode["Scale"]);
				}

				// Mesh
				if (entityNode["MeshComponent"])
				{
					auto& meshComp = entity.AddComponent<MeshComponent>();
					auto meshNode = entityNode["MeshComponent"];

					if (meshNode["Mesh"])
					{
						uint64_t meshID = meshNode["Mesh"].as<uint64_t>();
						meshComp.mesh = AssetManager::GetAsset<Mesh>(meshID);
					}

					if (meshNode["Material"])
					{
						uint64_t materialID = meshNode["Material"].as<uint64_t>();
						meshComp.material = AssetManager::GetAsset<Material>(materialID);
					}
				}

				// TODO: Camera, Light, Audio, etc.
			}
		}

		return scene;
	}
}