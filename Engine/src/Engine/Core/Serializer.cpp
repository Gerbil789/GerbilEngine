#include "enginepch.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Engine 
{
	void Serializer::Serialize(const Ref<Material>& material)
	{
		if (!material)
		{
			LOG_ERROR("Material is null, cannot serialize.");
			return;
		}

		YAML::Emitter out;

		out << YAML::BeginMap;

		auto shader = material->GetShader();
		WriteString(out, SHADER_KEY, (shader ? shader->GetFilePath().string() : ""));

		out << YAML::Key << PROPERTIES_KEY << YAML::Value << YAML::BeginMap;

		//auto properties = shader->GetUniformBuffer();
		//for (auto& element : properties)
		//{
		//	std::string name = element.Name;

		//	if (!material->HasProperty(name))
		//	{
		//		LOG_ERROR("Material does not have property: {0}", name);
		//		continue;
		//	}

		//	auto type = element.Type;

		//	if (type == ShaderDataType::Float4)
		//	{
		//		WriteVec4(out, name, material->GetProperty<glm::vec4>(name));
		//	}
		//	else if (type == ShaderDataType::Float3)
		//	{
		//		WriteVec3(out, name, material->GetProperty<glm::vec3>(name));
		//	}
		//	else if (type == ShaderDataType::Float2)
		//	{
		//		WriteVec2(out, name, material->GetProperty<glm::vec2>(name));
		//	}
		//	else if (type == ShaderDataType::Float)
		//	{
		//		WriteFloat(out, name, material->GetProperty<float>(name));
		//	}
		//	else if (type == ShaderDataType::Int)
		//	{
		//		WriteInt(out, name, material->GetProperty<int>(name));
		//	}
		//	//else if (type == ShaderDataType::Texture2D)
		//	//{
		//	//	//writeAssetPath(name, material->GetProperty<Ref<Texture2D>>(name));
		//	//}
		//	else
		//	{
		//		LOG_ERROR("Unknown material property type!");
		//	}
		//
		//}

		//auto properties = material->GetProperties();
		/*for (auto& property : properties)
		{
			std::string name = property.first;
			auto value = property.second;

			if (std::holds_alternative<glm::vec4>(value))
			{
				WriteVec4(out, name, std::get<glm::vec4>(value));
			}
			else if (std::holds_alternative<glm::vec3>(value))
			{
				WriteVec3(out, name, std::get<glm::vec3>(value));
			}
			else if (std::holds_alternative<glm::vec2>(value))
			{
				WriteVec2(out, name, std::get<glm::vec2>(value));
			}
			else if (std::holds_alternative<float>(value))
			{
				WriteFloat(out, name, std::get<float>(value));
			}
			else if (std::holds_alternative<int>(value))
			{
				WriteInt(out, name, std::get<int>(value));
			}
			else if (std::holds_alternative<Ref<Texture2D>>(value))
			{
				writeAssetPath(name, std::get<Ref<Texture2D>>(value));
			}
			else
			{
				LOG_ERROR("Unknown material property type!");
			}
		}*/

		out << YAML::EndMap; //properties
		out << YAML::EndMap;

		std::ofstream fout(material->GetFilePath());
		if (!fout.is_open())
		{
			LOG_ERROR("Failed to open file for serialization: {0}", material->GetFilePath());
			return;
		}
		fout << out.c_str();
		fout.close();
	}

	bool Serializer::Deserialize(Ref<Material>& material)
	{
		std::filesystem::path path = material->GetFilePath();
		std::ifstream stream(path);
		if (!stream.is_open())
		{
			LOG_ERROR("Failed to open file {0}", path);
			return false;
		}

		YAML::Node data = YAML::LoadFile(path.string());
		if (!data[SHADER_KEY].IsDefined())
		{
			LOG_ERROR("Shader field missing in material! {0}", path);
			return false;
		}

		auto loadTexture = [&](const std::string& key) -> Ref<Texture2D> {
			if (data[key].IsNull()) { return nullptr; }
			return AssetManager::GetAsset<Texture2D>(data[key].as<std::string>());
			};

		auto shader = AssetManager::GetAsset<Shader>(data[SHADER_KEY].as<std::string>());
		material->SetShader(shader);

		auto materialProperties = shader->GetMaterialBufferLayout();

		//auto shaderProperties = shader->GetUniformBuffers(); //TODO: check if properties of material and shader match
		//auto materialProperties = data[PROPERTIES_KEY];

		for (auto& property : materialProperties)
		{
			std::string name = property.Name;
			auto type = property.Type;

			if (!data[PROPERTIES_KEY][name].IsDefined())
			{
				continue;
			}

			if (type == ShaderDataType::Float4)
			{
				material->SetProperty(name, ReadVec4(data[PROPERTIES_KEY], name));
			}
			else if (type == ShaderDataType::Float3)
			{
				material->SetProperty(name, ReadVec3(data[PROPERTIES_KEY], name));
			}
			else if (type == ShaderDataType::Float2)
			{
				material->SetProperty(name, ReadVec2(data[PROPERTIES_KEY], name));
			}
			else if (type == ShaderDataType::Float)
			{
				material->SetProperty(name, ReadFloat(data[PROPERTIES_KEY], name));
			}
			else if (type == ShaderDataType::Int)
			{
				material->SetProperty(name, ReadInt(data[PROPERTIES_KEY], name));
			}
			//else if (type == ShaderDataType::Texture2D)
			//{
			//	//writeAssetPath(name, material->GetProperty<Ref<Texture2D>>(name));
			//}
			else
			{
				LOG_ERROR("Unknown material property type!");
			}
			
		}



		material->SetModified(false);
		return true;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		ASSERT(entity.HasComponent<IDComponent>(), "Entity has no IdComponent!");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		out << YAML::Key << "Name" << YAML::Value << entity.GetComponent<NameComponent>().Name;
		out << YAML::Key << "Enabled" << YAML::Value << entity.GetComponent<EnablingComponent>().Enabled;
		out << YAML::Key << "Transform";

		out << YAML::BeginMap;
		auto& tc = entity.GetComponent<TransformComponent>();
		out << YAML::Key << "Position" << YAML::Flow << YAML::BeginSeq << tc.Position.x << tc.Position.y << tc.Position.z << YAML::EndSeq;
		out << YAML::Key << "Rotation" << YAML::Flow << YAML::BeginSeq << tc.Rotation.x << tc.Rotation.y << tc.Rotation.z << YAML::EndSeq;
		out << YAML::Key << "Scale" << YAML::Flow << YAML::BeginSeq << tc.Scale.x << tc.Scale.y << tc.Scale.z << YAML::EndSeq;
		out << YAML::EndMap;

		// Camera Component
		if (entity.HasComponent<CameraComponent>()) 
		{
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
		if (entity.HasComponent<SpriteRendererComponent>()) 
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Material" << YAML::Value << (src.Material ? src.Material->GetFilePath().string() : "null");
			out << YAML::Key << "Texture" << YAML::Value << (src.Texture ? src.Texture->GetFilePath().string() : "null");
			out << YAML::Key << "Color" << YAML::Flow << YAML::BeginSeq << src.Color.r << src.Color.g << src.Color.b << src.Color.a << YAML::EndSeq;
			out << YAML::Key << "TilingFactor" << YAML::Flow << YAML::BeginSeq << src.TilingFactor.x << src.TilingFactor.y << YAML::EndSeq;
			out << YAML::EndMap;
		}

		// Light Component
		if (entity.HasComponent<LightComponent>()) 
		{
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;
			auto& lc = entity.GetComponent<LightComponent>();
			out << YAML::Key << "Type" << YAML::Value << (int)lc.Type;
			out << YAML::Key << "Color" << YAML::Flow << YAML::BeginSeq << lc.Color.r << lc.Color.g << lc.Color.b << YAML::EndSeq;
			out << YAML::Key << "Intensity" << YAML::Value << lc.Intensity;
			out << YAML::Key << "Range" << YAML::Value << lc.Range;
			out << YAML::Key << "Attenuation" << YAML::Flow << YAML::BeginSeq << lc.Attenuation.x << lc.Attenuation.y << lc.Attenuation.z << YAML::EndSeq;
			out << YAML::Key << "InnerAngle" << YAML::Value << lc.InnerAngle;
			out << YAML::Key << "OuterAngle" << YAML::Value << lc.OuterAngle;
			out << YAML::EndMap;
		}

		// Mesh Renderer Component
		if (entity.HasComponent<MeshRendererComponent>())
		{
			out << YAML::Key << "MeshRendererComponent";
			out << YAML::BeginMap;
			auto& mrc = entity.GetComponent<MeshRendererComponent>();
			out << YAML::Key << "Mesh" << YAML::Value << (mrc.Mesh ? mrc.Mesh->GetFilePath().string() : "null");
			out << YAML::Key << "Material" << YAML::Value << (mrc.Material ? mrc.Material->GetFilePath().string() : "null");
			out << YAML::EndMap;
		}

		// --- END ---
		out << YAML::EndMap;
	}


	void Serializer::Serialize(const Ref<Scene>& scene)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << scene->GetFilePath().filename().string();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		std::vector<Entity> entites = scene->GetEntities();
		for(const Entity& entity : entites)
		{
			SerializeEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(scene->GetFilePath());
		fout << out.c_str();
		fout.close();
	}



	bool Serializer::Deserialize(Ref<Scene>& scene)
	{
		std::string filePath = scene->GetFilePath().string();
		std::ifstream stream(filePath);
		if (!stream.is_open())
		{
			LOG_ERROR("Failed to open file '{0}'", filePath);
			return false;
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filePath);
		}
		catch (YAML::ParserException e)
		{
			LOG_ERROR("Failed to load .scene file '{0}'\n{1}", filePath, e.what());
			return false;
		}
		if (!data["Scene"]) return false;

		std::string sceneName = data["Scene"].as<std::string>();
		LOG_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities) {
			LOG_WARNING("No entities found in scene file");
			return true;
		}

		for (auto entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();
			std::string name = entity["Name"].as<std::string>();
			LOG_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = scene->CreateEntity(uuid, name);

			auto transform = entity["Transform"];
			if (transform)
			{
				auto& tc = deserializedEntity.GetComponent<TransformComponent>();
				tc.Position = { transform["Position"][0].as<float>(), transform["Position"][1].as<float>(), transform["Position"][2].as<float>() };
				tc.Rotation = { transform["Rotation"][0].as<float>(), transform["Rotation"][1].as<float>(), transform["Rotation"][2].as<float>() };
				tc.Scale = { transform["Scale"][0].as<float>(), transform["Scale"][1].as<float>(), transform["Scale"][2].as<float>() };
			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto& cc = deserializedEntity.AddComponent<CameraComponent>();

				auto cameraProps = cameraComponent["Camera"];
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
				std::string materialPath = spriteRendererComponent["Material"].as<std::string>();
				if (materialPath != "null") src.Material = AssetManager::GetAsset<Material>(materialPath);

				std::string texturePath = spriteRendererComponent["Texture"].as<std::string>();
				if (texturePath != "null") src.Texture = AssetManager::GetAsset<Texture2D>(texturePath);
				
				src.Color = { spriteRendererComponent["Color"][0].as<float>(), spriteRendererComponent["Color"][1].as<float>(), spriteRendererComponent["Color"][2].as<float>(), spriteRendererComponent["Color"][3].as<float>() };
				src.TilingFactor = { spriteRendererComponent["TilingFactor"][0].as<float>(), spriteRendererComponent["TilingFactor"][1].as<float>() };
			}

			auto lightComponent = entity["LightComponent"];
			if (lightComponent)
			{
				auto& lc = deserializedEntity.AddComponent<LightComponent>();
				lc.Type = (LightType)lightComponent["Type"].as<int>();
				lc.Color = { lightComponent["Color"][0].as<float>(), lightComponent["Color"][1].as<float>(), lightComponent["Color"][2].as<float>() };
				lc.Intensity = lightComponent["Intensity"].as<float>();
				lc.Range = lightComponent["Range"].as<float>();
				lc.Attenuation.x = lightComponent["Attenuation"][0].as<float>();
				lc.Attenuation.y = lightComponent["Attenuation"][1].as<float>();
				lc.Attenuation.z = lightComponent["Attenuation"][2].as<float>();
				lc.InnerAngle = lightComponent["InnerAngle"].as<float>();
				lc.OuterAngle = lightComponent["OuterAngle"].as<float>();
			}

			auto meshRendererComponent = entity["MeshRendererComponent"];
			if (meshRendererComponent)
			{
				auto& mrc = deserializedEntity.AddComponent<MeshRendererComponent>();
				std::string meshPath = meshRendererComponent["Mesh"].as<std::string>();
				if (meshPath != "null") mrc.Mesh = AssetManager::GetAsset<Mesh>(meshPath);

				std::string materialPath = meshRendererComponent["Material"].as<std::string>();
				if (materialPath != "null") mrc.Material = AssetManager::GetAsset<Material>(materialPath);
			}
		}

		return true;
	}

	void Serializer::WriteVec4(YAML::Emitter& out, const std::string& key, const glm::vec4& vec) 
	{
		out << YAML::Key << key << YAML::Value << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
	}

	void Serializer::WriteVec3(YAML::Emitter& out, const std::string& key, const glm::vec3& vec) 
	{
		out << YAML::Key << key << YAML::Value << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
	}

	void Serializer::WriteVec2(YAML::Emitter& out, const std::string& key, const glm::vec2& vec) 
	{
		out << YAML::Key << key << YAML::Value << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
	}

	void Serializer::WriteFloat(YAML::Emitter& out, const std::string& key, float value) 
	{
		out << YAML::Key << key << YAML::Value << value;
	}

	void Serializer::WriteInt(YAML::Emitter& out, const std::string& key, int value) 
	{
		out << YAML::Key << key << YAML::Value << value;
	}

	void Serializer::WriteString(YAML::Emitter& out, const std::string& key, const std::string& value) 
	{
		out << YAML::Key << key;

		if (value.empty()) 
		{
			out << YAML::Null;
		}
		else 
		{
			out << YAML::Value << value; 
		}
	}

	glm::vec4 Serializer::ReadVec4(const YAML::Node& node, const std::string& key)
	{
		return glm::vec4 { node[key][0].as<float>(), node[key][1].as<float>(), node[key][2].as<float>(), node[key][3].as<float>() };
	}

	glm::vec3 Serializer::ReadVec3(const YAML::Node& node, const std::string& key)
	{
		return glm::vec3 { node[key][0].as<float>(), node[key][1].as<float>(), node[key][2].as<float>() };
	}

	glm::vec2 Serializer::ReadVec2(const YAML::Node& node, const std::string& key)
	{
		return glm::vec2 { node[key][0].as<float>(), node[key][1].as<float>() };
	}

	float Serializer::ReadFloat(const YAML::Node& node, const std::string& key)
	{
		float value = node[key].as<float>();
		return value;
	}

	int Serializer::ReadInt(const YAML::Node& node, const std::string& key)
	{
		int value = node[key].as<int>();
		return value;
	}

	std::string Serializer::ReadString(const YAML::Node& node, const std::string& key)
	{
		return node[key].as<std::string>();
	}
}