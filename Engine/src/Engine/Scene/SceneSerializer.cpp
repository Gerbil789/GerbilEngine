#include "enginepch.h"
#include "Engine/Scene/SceneSerializer.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"


namespace YAML 
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	//template<>
	//struct convert<Engine::UUID>
	//{
	//	static Node encode(const Engine::UUID& uuid)
	//	{
	//		Node node;
	//		node.push_back((uint64_t)uuid);
	//		return node;
	//	}

	//	static bool decode(const Node& node, Engine::UUID& uuid)
	//	{
	//		uuid = node.as<uint64_t>();
	//		return true;
	//	}
	//};
}


namespace Engine 
{
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

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) {}


	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		ASSERT(entity.HasComponent<IDComponent>(), "Entity has no IdComponent!");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		out << YAML::Key << "Name" << YAML::Value << entity.GetComponent<NameComponent>().Name;

		out << YAML::Key << "Transform";
		out << YAML::BeginMap;
		auto& tc = entity.GetComponent<TransformComponent>();
		out << YAML::Key << "Position" << YAML::Value << tc.Position;
		out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
		out << YAML::EndMap;

		// Camera Component
		if (entity.HasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			auto& cc = entity.GetComponent<CameraComponent>();
			auto& camera = cc.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "Main" << YAML::Value << cc.Main;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;
			out << YAML::EndMap;
		}

		// Sprite Renderer Component
		if (entity.HasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << src.Color;
			out << YAML::Key << "TilingFactor" << YAML::Value << src.TilingFactor;
			out << YAML::EndMap;
		}

		// --- END ---
		out << YAML::EndMap;
	}


	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		for (const auto [ent, ref] : m_Scene->m_Registry.storage<NameComponent>().each()) 
		{
			Entity entity = { ent, m_Scene.get() };
			if (!entity) return;

			SerializeEntity(out, entity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}


	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			ENGINE_LOG_ERROR("Failed to load .scene file '{0}'\n     {1}", filepath, e.what());
			return false;
		}
		if (!data["Scene"]) return false;

		std::string sceneName = data["Scene"].as<std::string>();
		ENGINE_LOG_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities) {
			ENGINE_LOG_WARNING("No entities found in scene file");
			return true;
		}

		for (auto entity : entities) 
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();
			std::string name = entity["Name"].as<std::string>();
			ENGINE_LOG_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = m_Scene->CreateEntity(uuid, name);

			auto transform = entity["Transform"];
			if (transform)
			{
				auto& tc = deserializedEntity.GetComponent<TransformComponent>();
				tc.Position = transform["Position"].as<glm::vec3>();
				tc.Rotation = transform["Rotation"].as<glm::vec3>();
				tc.Scale = transform["Scale"].as<glm::vec3>();
			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto& cc = deserializedEntity.AddComponent<CameraComponent>();

				auto& cameraProps = cameraComponent["Camera"];
				cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

				cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
				cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
				cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

				cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
				cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
				cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

				cc.Main = cameraComponent["Main"].as<bool>();
				cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			}

			auto spriteRendererComponent = entity["SpriteRendererComponent"];
			if (spriteRendererComponent)
			{
				auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
				src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				src.TilingFactor = spriteRendererComponent["TilingFactor"].as<glm::vec2>();
			}
		}

		return true;
		
	}
}