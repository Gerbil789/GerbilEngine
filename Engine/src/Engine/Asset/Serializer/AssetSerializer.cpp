#include "enginepch.h"
#include "AssetSerializer.h"

#include "Engine/Core/Project.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"

namespace Engine
{
	using AssetSerializeFunction = std::function<void(Asset* asset, const std::filesystem::path&)>;
	using AssetDeserializeFunction = std::function<Asset*(const std::filesystem::path&)>;

	static std::map<AssetType, AssetSerializeFunction> s_AssetSerializeFunctions = {
		{ AssetType::Scene, [](Asset* asset, const std::filesystem::path& path) { SceneSerializer::Serialize(static_cast<Scene*>(asset), path); }},
		{ AssetType::Material, [](Asset* asset, const std::filesystem::path& path) { MaterialSerializer::Serialize(static_cast<Material*>(asset), path); }}
	};

	static std::map<AssetType, AssetDeserializeFunction> s_AssetDeserializeFunctions = {
		{ AssetType::Scene, [](const std::filesystem::path& path) -> Asset* { return SceneSerializer::Deserialize(path); }},
		{ AssetType::Material, [](const std::filesystem::path& path) -> Asset* { return MaterialSerializer::Deserialize(path); }}
	};

	void AssetSerializer::SerializeAsset(Asset* asset, const AssetRecord& metadata)
	{
		AssetType assetType = metadata.type;

		if (s_AssetSerializeFunctions.find(assetType) == s_AssetSerializeFunctions.end())
		{
			LOG_ERROR("No importer available for asset type: {}", AssetTypeToString(assetType));
			return;
		}

		return s_AssetSerializeFunctions.at(assetType)(asset, Project::GetAssetsDirectory() / metadata.path);
	}

	Asset* AssetSerializer::DeserializeAsset(const AssetRecord& metadata)
	{
		AssetType assetType = metadata.type;

		if (s_AssetDeserializeFunctions.find(assetType) == s_AssetDeserializeFunctions.end())
		{
			LOG_ERROR("No importer available for asset type: {}", AssetTypeToString(assetType));
			return nullptr;
		}

		return s_AssetDeserializeFunctions.at(assetType)(Project::GetAssetsDirectory() / metadata.path);
	}

}