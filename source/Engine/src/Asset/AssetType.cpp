#include "enginepch.h"
#include "Engine/Asset/AssetType.h"

namespace engine
{
	// FNV-1a 32-bit hash algorithm
	// converts to lowercase 
	constexpr uint32_t HashExtension(std::string_view str)
	{
		uint32_t hash = 0x811c9dc5;
		for (char c : str)
		{
			char lowerC = (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
			hash ^= static_cast<uint32_t>(lowerC);
			hash *= 0x01000193;
		}
		return hash;
	}

	//TODO: rename files to lowercase extensions instead of chicking it all the time
	AssetType GetAssetTypeFromExtension(std::string_view extension)
	{
		switch (HashExtension(extension))
		{
		case HashExtension(".png"):
		case HashExtension(".jpg"):
		case HashExtension(".jpeg"):
		case HashExtension(".hdr"):   return AssetType::Texture;

		case HashExtension(".gltf"):
		case HashExtension(".glb"):   return AssetType::Mesh;

		case HashExtension(".mat"):   return AssetType::Material;

		case HashExtension(".wav"):
		case HashExtension(".mp3"):   return AssetType::Audio;

		case HashExtension(".scene"): return AssetType::Scene;
		case HashExtension(".wgsl"):  return AssetType::Shader;

		default:                      return AssetType::Unknown;
		}
	}

	std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::Texture:  return "Texture";
		case AssetType::Mesh:     return "Mesh";
		case AssetType::Shader:   return "Shader";
		case AssetType::Material: return "Material";
		case AssetType::Audio:    return "Audio";
		case AssetType::Scene:    return "Scene";
		case AssetType::Script:   return "Script";
		default:                  return "Unknown";
		}
	}
}