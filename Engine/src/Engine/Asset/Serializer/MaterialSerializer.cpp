#include "enginepch.h"
#include "MaterialSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include <yaml-cpp/yaml.h>

namespace Engine
{
	//TODO: move to some shared serialization utils?
	void SerializeValue(YAML::Emitter& out, const std::string& name, float v)
	{
		out << YAML::Key << name << YAML::Value << v;
	}

	void SerializeValue(YAML::Emitter& out, const std::string& name, const glm::vec3& v)
	{
		out << YAML::Key << name << YAML::Value << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	}

	void SerializeValue(YAML::Emitter& out, const std::string& name, const glm::vec4& v)
	{
		out << YAML::Key << name << YAML::Value << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	}

	void SerializeValue(YAML::Emitter& out, const std::string& name, uint64_t v)
	{
		out << YAML::Key << name << YAML::Value << v;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////// SERIALIZATION ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	void SerializeUniformBuffer(YAML::Emitter& out, const Binding& binding, const std::vector<uint8_t>& data)
	{
		auto parameters = binding.parameters;
		for (const auto& param : parameters)
		{
			switch (param.type)
			{
			case ShaderValueType::Float:
			{
				float value;
				std::memcpy(&value, data.data() + param.offset, sizeof(float));
				SerializeValue(out, param.name, value);
				break;
			}
			case ShaderValueType::Vec3:
			{
				glm::vec3 value;
				std::memcpy(&value, data.data() + param.offset, sizeof(glm::vec3));
				SerializeValue(out, param.name, value);
				break;
			}
			case ShaderValueType::Vec4:
			{
				glm::vec4 value;
				std::memcpy(&value, data.data() + param.offset, sizeof(glm::vec4));
				SerializeValue(out, param.name, value);
				break;
			}
			default:
				LOG_ERROR("MaterialSerializer::Serialize - Unsupported uniform type for parameter '{}'", param.name);
			}
		}
	}

	void MaterialSerializer::Serialize(Material* material, const std::filesystem::path& path)
	{
		if (!material)
		{
			LOG_ERROR("Material is null, cannot serialize.");
			return;
		}

		auto shader = material->GetShader();
		if (!shader)
		{
			LOG_ERROR("Material has no shader, cannot serialize.");
			return;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;

		// Shader ID
		out << YAML::Key << "Shader" << YAML::Value << shader->id;

		auto shaderSpec = shader->GetSpecification();
		auto materialBindings = GetMaterialBindings(shaderSpec);

		// Attributes (floats, vec3, vec4)
		out << YAML::Key << "Attributes" << YAML::Value << YAML::BeginMap;
		for (const Binding& binding : materialBindings)
		{
			if (binding.type == BindingType::UniformBuffer)
				SerializeUniformBuffer(out, binding, material->m_UniformData);
		}
		out << YAML::EndMap;

		// Textures (Texture2D)
		out << YAML::Key << "Textures" << YAML::Value << YAML::BeginMap;
		for (const Binding& binding : materialBindings)
		{
			if (binding.type != BindingType::Texture2D)
				continue;

			auto it = material->m_Textures.find(binding.name);
			if (it == material->m_Textures.end())
			{
				LOG_WARNING("MaterialSerializer::Serialize - Texture '{}' not found!", binding.name);
				out << YAML::Key << binding.name << YAML::Value << "null";
				continue;
			}

			auto texture = it->second;
			SerializeValue(out, binding.name, texture->id);
		}
		out << YAML::EndMap;

		out << YAML::EndMap; // End root

		// Write to file
		std::ofstream fout(path);
		if (!fout.is_open())
		{
			LOG_ERROR("Failed to open file for serialization: {0}", path);
			return;
		}
		fout << out.c_str();
		fout.close();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////// DESERIALIZATION //////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	//TODO: move to some shared serialization utils?
	bool DeserializeValue(const YAML::Node& node, float& v)
	{
		if (!node.IsScalar()) return false;
		v = node.as<float>();
		return true;
	}

	bool DeserializeValue(const YAML::Node& node, glm::vec3& v)
	{
		if (!node.IsSequence() || node.size() != 3) return false;
		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();
		return true;
	}

	bool DeserializeValue(const YAML::Node& node, glm::vec4& v)
	{
		if (!node.IsSequence() || node.size() != 4) return false;
		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();
		v.w = node[3].as<float>();
		return true;
	}

	bool DeserializeValue(const YAML::Node& node, uint64_t& v)
	{
		if (!node.IsScalar()) return false;
		v = node.as<uint64_t>();
		return true;
	}


	Material* MaterialSerializer::Deserialize(const std::filesystem::path& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (const YAML::Exception& e)
		{
			LOG_ERROR("YAML parse error in {}: {}", path, e.what());
			return nullptr;
		}


		auto shader = AssetManager::GetAsset<Shader>(UUID(data["Shader"].as<uint64_t>()));

		if(!shader)
		{
			LOG_ERROR("MaterialSerializer::Deserialize - Failed to load shader with ID: {0}", data["Shader"].as<uint64_t>());
			return nullptr;
		}

		auto material = new Material(shader);

		if (auto attributes = data["Attributes"])
		{
			for (auto it = attributes.begin(); it != attributes.end(); ++it)
			{
				std::string name = it->first.as<std::string>();
				const YAML::Node& node = it->second;

				// Try float first
				float f;
				if (DeserializeValue(node, f))
				{
					material->SetFloat(name, f);
					continue;
				}

				//glm::vec3 v3;
				//if (DeserializeValue(node, v3))
				//{
				//	material->set(name, v3);
				//	continue;
				//}

				glm::vec4 v4;
				if (DeserializeValue(node, v4))
				{
					material->SetVec4(name, v4);
					continue;
				}

				LOG_WARNING("Unknown property format for '{}'", name);
			}
		}

		if (auto textures = data["Textures"])
		{
			for (auto it = textures.begin(); it != textures.end(); ++it)
			{
				std::string name = it->first.as<std::string>();
				const YAML::Node& node = it->second;

				uint64_t texID;
				if (DeserializeValue(node, texID))
				{
					auto texture = AssetManager::GetAsset<Texture2D>(UUID(texID));
					if (texture)
						material->SetTexture(name, texture);
					else
						LOG_WARNING("MaterialSerializer::Deserialize - Failed to load texture '{}' with ID: {}", name, texID);
				}
				else
				{
					LOG_WARNING("Texture '{}' has invalid format", name);
				}
			}
		}

		return material;
	}
}