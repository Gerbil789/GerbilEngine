#include "enginepch.h"
#include "Serializer.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Material.h"
#include <regex>
#include <json.hpp>

using json = nlohmann::ordered_json;

namespace Engine::Serializer
{
	void Serialize(const Ref<Material>& material)
	{
		if (!material)
		{
			LOG_ERROR("Material is null, cannot serialize.");
			return;
		}

		//YAML::Emitter out;
		//out << YAML::BeginMap;

		//auto shader = material->GetShader();
		////writer.Write(SHADER_KEY, (shader ? shader->GetFilePath().string() : ""));

		//out << YAML::Key << PROPERTIES_KEY << YAML::Value << YAML::BeginMap;

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

		//out << YAML::EndMap; //properties
		//out << YAML::EndMap;

		//std::ofstream fout(material->GetFilePath());
		//if (!fout.is_open())
		//{
		//	LOG_ERROR("Failed to open file for serialization: {0}", material->GetFilePath());
		//	return;
		//}
		////fout << out.c_str();
		//fout.close();
	}

	bool Serializer::Deserialize(Ref<Material>& material)
	{
		std::filesystem::path path = material->GetPath();
		std::ifstream stream(path);
		if (!stream.is_open())
		{
			LOG_ERROR("Failed to open file {0}", path);
			return false;
		}

		/*YAML::Node data = YAML::LoadFile(path.string());
		if (!data[SHADER_KEY].IsDefined())
		{
			LOG_ERROR("Shader field missing in material! {0}", path);
			return false;
		}

		auto loadTexture = [&](const std::string& key) -> Ref<Texture2D> {
			if (data[key].IsNull()) { return nullptr; }
			return AssetManager::Get<Texture2D>(data[key].as<std::string>());
			};*/

		//auto shader = AssetManager::GetAsset<Shader>(data[SHADER_KEY].as<std::string>());
		//auto shader = CreateRef<Shader>();
		//material->SetShader(shader);

		//auto materialProperties = shader->GetMaterialBufferLayout();

		//auto shaderProperties = shader->GetUniformBuffers(); //TODO: check if properties of material and shader match
		//auto materialProperties = data[PROPERTIES_KEY];

		//for (auto& property : materialProperties)
		//{
		//	std::string name = property.Name;
		//	auto type = property.Type;

		//	if (!data[PROPERTIES_KEY][name].IsDefined())
		//	{
		//		continue;
		//	}

		//	if (type == ShaderDataType::Float4)
		//	{
		//		material->SetProperty(name, ReadVec4(data[PROPERTIES_KEY], name));
		//	}
		//	else if (type == ShaderDataType::Float3)
		//	{
		//		material->SetProperty(name, ReadVec3(data[PROPERTIES_KEY], name));
		//	}
		//	else if (type == ShaderDataType::Float2)
		//	{
		//		material->SetProperty(name, ReadVec2(data[PROPERTIES_KEY], name));
		//	}
		//	else if (type == ShaderDataType::Float)
		//	{
		//		material->SetProperty(name, ReadFloat(data[PROPERTIES_KEY], name));
		//	}
		//	else if (type == ShaderDataType::Int)
		//	{
		//		material->SetProperty(name, ReadInt(data[PROPERTIES_KEY], name));
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

		//material->SetModified(false);
		return true;
	}

	void Serializer::Serialize(Scene* scene)
	{
		json j;

		j["Scene"] = scene->GetPath().string();
		j["Entities"] = json::array();

		for (const Entity& entity : scene->GetEntities())
		{
			json entityJson;
			entityJson["Entity"] = entity.GetComponent<NameComponent>().Name;
			entityJson["ID"] = (uint32_t)entity.GetComponent<IdentityComponent>().ID;
			entityJson["Enabled"] = entity.GetComponent<IdentityComponent>().Enabled;

			json transformJson;
			const auto& transform = entity.GetComponent<TransformComponent>();
			transformJson["Position"] = { transform.Position.x, transform.Position.y, transform.Position.z };
			transformJson["Rotation"] = { transform.Rotation.x, transform.Rotation.y, transform.Rotation.z };
			transformJson["Scale"] = { transform.Scale.x, transform.Scale.y, transform.Scale.z };
			entityJson["Transform"] = transformJson;

			// Components
			json componentsJson = json::array();

			// MeshComponent
			if(auto* component = entity.TryGetComponent<MeshComponent>())
			{
				json data;
				data["Mesh"] = component->Mesh ? component->Mesh->GetPath() : "";
				data["Material"] = component->Material ? component->Material->GetPath() : "";
				componentsJson.push_back({ { "MeshComponent", data } });
			}

			// Other components here...


			entityJson["Components"] = componentsJson;
			j["Entities"].push_back(entityJson);
		}

		std::ofstream outFile(scene->GetPath());
		if (!outFile.is_open())
		{
			LOG_ERROR("Failed to open file for serialization: {0}", scene->GetPath());
			return;
		}

		std::string output = j.dump(2);
		output = std::regex_replace(output, std::regex(R"(\[\s+(-?\d+\.?\d*),\s+(-?\d+\.?\d*),\s+(-?\d+\.?\d*)\s+\])"), "[$1, $2, $3]"); // Post-process: collapse float3 arrays to one line using regex
		outFile << output;
	}

	bool Serializer::Deserialize(Scene* scene)
	{
	
		return true;
	}
}