#include "enginepch.h"
#include "AssetImporter.h"
#include <Engine/Asset/AssetMetadata.h>
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/SceneImporter.h"

namespace Engine
{
	using AssetImportFunction = std::function<Ref<Asset>(UUID, const AssetMetadata&)>;

	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{ AssetType::Texture, TextureImporter::ImportTexture2D },
		{ AssetType::Mesh, MeshImporter::ImportMesh },
		{ AssetType::Material, MaterialImporter::ImportMaterial },
		{ AssetType::Scene, SceneImporter::ImportScene }
	};

	Ref<Asset> AssetImporter::ImportAsset(UUID id, const AssetMetadata& metadata)
	{
		AssetType assetType = metadata.GetType();

		if (s_AssetImportFunctions.find(assetType) == s_AssetImportFunctions.end())
		{
			LOG_ERROR("No importer available for asset type: {}", AssetTypeToString(assetType));
			return nullptr;
		}

		return s_AssetImportFunctions.at(assetType)(id, metadata);
	}

}