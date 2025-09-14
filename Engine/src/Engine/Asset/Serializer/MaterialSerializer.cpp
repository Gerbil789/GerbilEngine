#include "enginepch.h"
#include "MaterialSerializer.h"
#include <yaml-cpp/yaml.h>

namespace Engine
{
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

	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////// SERIALIZATION ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	void MaterialSerializer::Serialize(const Ref<Material>& material, const std::filesystem::path& path)
	{
		if (!material)
		{
			LOG_ERROR("Material is null, cannot serialize.");
			return;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;

		//Ref<Shader> shader = material->GetShader();
		//out << YAML::Key << "Shader" << YAML::Value << shader->GetParameters().;

		out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;

		//const auto& values = material->GetValues();
		//for (auto& [name, value] : values)
		//{
		//	std::visit([&](auto&& arg) {
		//		SerializeValue(out, name, arg);
		//		}, value);
		//
		//}
		//out << YAML::EndMap; // End Properties
		out << YAML::EndMap; // End root

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


	Ref<Material> MaterialSerializer::Deserialize(const std::filesystem::path& path)
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

		auto material = CreateRef<Material>();

		//if (auto props = data["Properties"])
		//{
		//	for (auto it = props.begin(); it != props.end(); ++it)
		//	{
		//		std::string name = it->first.as<std::string>();
		//		const YAML::Node& node = it->second;

		//		// Try float first
		//		float f;
		//		if (DeserializeValue(node, f))
		//		{
		//			material->SetValue(name, f);
		//			continue;
		//		}

		//		glm::vec3 v3;
		//		if (DeserializeValue(node, v3))
		//		{
		//			material->SetValue(name, v3);
		//			continue;
		//		}

		//		glm::vec4 v4;
		//		if (DeserializeValue(node, v4))
		//		{
		//			material->SetValue(name, v4);
		//			continue;
		//		}

		//		LOG_WARNING("Unknown property format for '{}'", name);
		//	}
		//}

		return material;
	}
}