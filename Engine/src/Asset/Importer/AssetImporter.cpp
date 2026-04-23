#include "enginepch.h"
#include "Engine/Core/Project.h"
#include "Engine/Asset/Importer/AssetImporter.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Importer/AudioImporter.h"
#include "Engine/Asset/AssetRecord.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Audio/AudioClip.h"

namespace Engine
{
	using AssetImportFunction = std::function<Asset*(const std::filesystem::path&)>;

	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions
	{
		{ AssetType::Texture2D, TextureImporter::ImportTexture2D },
		//{ AssetType::CubeMap, TextureImporter::ImportCubeTexture },
		{ AssetType::Mesh, MeshImporter::ImportMesh },
		{ AssetType::Material, MaterialImporter::ImportMaterial },
		{ AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Shader, ShaderImporter::ImportShader },
		{ AssetType::Audio, AudioImporter::ImportAudio },
	};

	Asset* AssetImporter::ImportAsset(const AssetRecord& record)
	{
		if (record.path.empty() || !std::filesystem::exists(record.path))
		{
			LOG_ERROR("Invalid asset path: {}", record.path);
			return nullptr;
		}

		AssetType assetType = record.type;

		if (s_AssetImportFunctions.find(assetType) == s_AssetImportFunctions.end())
		{
			LOG_ERROR("No importer available for asset type: {}", AssetTypeToString(assetType));
			return nullptr;
		}

		auto path = Engine::Project::GetActive()->GetAssetsDirectory() / record.path;

		Asset* asset = s_AssetImportFunctions.at(assetType)(path);
		asset->id = record.id;
		return asset;
	}
}