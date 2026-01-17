#include "enginepch.h"
#include "AssetImporter.h"

#include "Engine/Core/Project.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Importer/AudioImporter.h"

namespace Engine
{
	using AssetImportFunction = std::function<Asset*(const AssetRecord&)>;

	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{ AssetType::Texture2D, TextureImporter::ImportTexture2D },
		{ AssetType::CubeMap, TextureImporter::ImportCubeMapTexture },
		{ AssetType::Mesh, MeshImporter::ImportMesh },
		{ AssetType::Material, MaterialImporter::ImportMaterial },
		{ AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Shader, ShaderImporter::ImportShader },
		{ AssetType::Audio, AudioImporter::ImportAudio }
	};

	Asset* AssetImporter::ImportAsset(const AssetRecord& metadata)
	{
		AssetType assetType = metadata.type;

		if (s_AssetImportFunctions.find(assetType) == s_AssetImportFunctions.end())
		{
			LOG_ERROR("No importer available for asset type: {}", AssetTypeToString(assetType));
			return nullptr;
		}
		Asset* asset = s_AssetImportFunctions.at(assetType)(metadata);
		asset->id = metadata.id;
		return asset;
	}
}