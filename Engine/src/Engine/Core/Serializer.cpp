#include "enginepch.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Core/AssetManager.h"
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Engine 
{
	void Serializer::Serialize(const Ref<Material>& material, const std::string& filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << material->shaderName;
		out << YAML::Key << "SurfaceType" << YAML::Value << (int)material->surfaceType;
		out << YAML::Key << "ColorTexture" << YAML::Value << ((material->colorTexture) ? material->colorTexture->GetFilePath() : "null");
		out << YAML::Key << "Color" << YAML::Value << YAML::Flow << YAML::BeginSeq << material->color.r << material->color.g << material->color.b << material->color.a << YAML::EndSeq;
		out << YAML::Key << "MetallicTexture" << YAML::Value << ((material->metallicTexture) ? material->metallicTexture->GetFilePath() : "null");
		out << YAML::Key << "Metallic" << YAML::Value << material->metallic;
		out << YAML::Key << "RoughnessTexture" << YAML::Value << ((material->roughnessTexture) ? material->roughnessTexture->GetFilePath() : "null");
		out << YAML::Key << "Roughness" << YAML::Value << material->roughness;
		out << YAML::Key << "NormalTexture" << YAML::Value << ((material->normalTexture) ? material->normalTexture->GetFilePath() : "null");
		out << YAML::Key << "NormalStrength" << YAML::Value << material->normalStrength;
		out << YAML::Key << "Tiling" << YAML::Value << YAML::Flow << YAML::BeginSeq << material->tiling.x << material->tiling.y << YAML::EndSeq;
		out << YAML::Key << "Offset" << YAML::Value << YAML::Flow << YAML::BeginSeq << material->offset.x << material->offset.y << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
		fout.close();
	}

	bool Serializer::Deserialize(const std::string& filePath, Ref<Material>& material)
	{
		std::ifstream stream(filePath);
		if (!stream.is_open())
		{
			ENGINE_LOG_ERROR("Failed to open file '{0}'", filePath);
			return false;
		}

		YAML::Node data = YAML::LoadFile(filePath);
		if (!data["Shader"]) return false;

		material = CreateRef<Material>();

		material->shaderName = data["Shader"].as<std::string>();
		material->surfaceType = (SurfaceType)data["SurfaceType"].as<int>();

		std::string colorTexturePath = data["ColorTexture"].as<std::string>();
		if(colorTexturePath != "null") material->colorTexture = AssetManager::LoadAsset<Texture2D>(colorTexturePath);

		material->color = glm::vec4(data["Color"][0].as<float>(), data["Color"][1].as<float>(), data["Color"][2].as<float>(), data["Color"][3].as<float>());

		std::string metallicTexturePath = data["MetallicTexture"].as<std::string>();
		if (metallicTexturePath != "null") material->metallicTexture = AssetManager::LoadAsset<Texture2D>(metallicTexturePath);
		
		material->metallic = data["Metallic"].as<float>();

		std::string roughnessTexturePath = data["RoughnessTexture"].as<std::string>();
		if (roughnessTexturePath != "null") material->roughnessTexture = AssetManager::LoadAsset<Texture2D>(roughnessTexturePath);

		material->roughness = data["Roughness"].as<float>();

		std::string normalTexturePath = data["NormalTexture"].as<std::string>();
		if (normalTexturePath != "null") material->normalTexture = AssetManager::LoadAsset<Texture2D>(normalTexturePath);

		material->normalStrength = data["NormalStrength"].as<float>();
		material->tiling = glm::vec2(data["Tiling"][0].as<float>(), data["Tiling"][1].as<float>());
		material->offset = glm::vec2(data["Offset"][0].as<float>(), data["Offset"][1].as<float>());

		return true;
		
	}
}