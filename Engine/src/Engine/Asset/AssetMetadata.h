#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Core/Log.h"
#include <filesystem>

namespace Engine
{
	enum class AssetType { Unknown = 0, Texture, Mesh, Material, Audio, Scene, Script };

	inline AssetType GetAssetTypeFromExtension(const std::string& extension)
	{
		std::string lower = extension;
		std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

		static const std::unordered_map<std::string, AssetType> extensionToType = {
				{ ".png",   AssetType::Texture },
				{ ".jpg",   AssetType::Texture },
				{ ".jpeg",  AssetType::Texture },
				{ ".gltf",  AssetType::Mesh },
				{ ".glb",   AssetType::Mesh },
				{ ".material",   AssetType::Material },
				{ ".wav",   AssetType::Audio },
				{ ".mp3",   AssetType::Audio },
				{ ".scene", AssetType::Scene }
		};

		if (auto it = extensionToType.find(lower); it != extensionToType.end())
			return it->second;

		LOG_WARNING("Unknown asset extension: {}", extension);
		return AssetType::Unknown;
	}

	inline std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::Unknown:  return "Unknown";
		case AssetType::Texture:  return "Texture";
		case AssetType::Mesh:     return "Mesh";
		case AssetType::Material: return "Material";
		case AssetType::Audio:    return "Audio";
		case AssetType::Scene:    return "Scene";
		case AssetType::Script:   return "Script";
		default:                  return "Unknown";
		}
	}

	inline AssetType AssetTypeFromString(std::string_view assetType)
	{
		if (assetType == "Texture")  return AssetType::Texture;
		if (assetType == "Mesh")     return AssetType::Mesh;
		if (assetType == "Material") return AssetType::Material;
		if (assetType == "Audio")    return AssetType::Audio;
		if (assetType == "Scene")    return AssetType::Scene;
		if (assetType == "Script")   return AssetType::Script;
		return AssetType::Unknown;
	}

	struct AssetMetadata
	{
		UUID id;
		std::filesystem::path path;

		AssetType GetType() const { return GetAssetTypeFromExtension(path.extension().string()); }
	};

	
};