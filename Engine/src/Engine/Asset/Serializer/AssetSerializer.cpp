#include "enginepch.h"
#include "AssetSerializer.h"

#include "Engine/Core/Project.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

namespace Engine
{
	using AssetSerializeFunction = std::function<void(const Ref<Asset>&, const std::filesystem::path&)>;
	using AssetDeserializeFunction = std::function<Ref<Asset>(const std::filesystem::path&)>;

	static std::map<AssetType, AssetSerializeFunction> s_AssetSerializeFunctions = {
		{ AssetType::Scene, [](const Ref<Asset>& asset, const std::filesystem::path& path) { SceneSerializer::Serialize(std::static_pointer_cast<Scene>(asset), path); }}
	};

	static std::map<AssetType, AssetDeserializeFunction> s_AssetDeserializeFunctions = {
		{ AssetType::Scene, [](const std::filesystem::path& path) -> Ref<Asset> { return SceneSerializer::Deserialize(path); }}
	};

	void AssetSerializer::SerializeAsset(const Ref<Asset>& asset, const AssetMetadata& metadata)
	{
		AssetType assetType = metadata.GetType();

		if (s_AssetSerializeFunctions.find(assetType) == s_AssetSerializeFunctions.end())
		{
			LOG_ERROR("No importer available for asset type: {}", AssetTypeToString(assetType));
			return;
		}

		return s_AssetSerializeFunctions.at(assetType)(asset, Project::GetAssetsDirectory() / metadata.path);
	}

	Ref<Asset> AssetSerializer::DeserializeAsset(const AssetMetadata& metadata)
	{
		AssetType assetType = metadata.GetType();

		if (s_AssetDeserializeFunctions.find(assetType) == s_AssetDeserializeFunctions.end())
		{
			LOG_ERROR("No importer available for asset type: {}", AssetTypeToString(assetType));
			return nullptr;
		}

		return s_AssetDeserializeFunctions.at(assetType)(Project::GetAssetsDirectory() / metadata.path);
	}

}