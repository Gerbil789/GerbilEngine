#include "enginepch.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Resources.h"

#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Audio/AudioClip.h"

#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Importer/AudioImporter.h"

namespace Engine
{
  namespace 
  {
    AssetRegistry m_AssetRegistry;
		std::filesystem::path m_AssetsDirectory;

    std::unordered_map<Uuid, Texture2D> m_Textures;
    std::unordered_map<Uuid, Mesh> m_Meshes;
    std::unordered_map<Uuid, Shader> m_Shaders;
    std::unordered_map<Uuid, Material> m_Materials;
		std::unordered_map<Uuid, AudioClip> m_AudioClips;
		std::unordered_map<Uuid, Scene> m_Scenes;

    std::optional<Texture2D> s_defaultTexture;
    std::optional<Mesh> s_defaultMesh;
    std::optional<Shader> s_defaultShader;
    std::optional<Material> s_defaultMaterial;
		std::optional<AudioClip> s_defaultAudioClip;
		std::optional<Scene> s_defaultScene;

    template<typename T, typename ImporterFunc>
    T& LoadAssetInternal(Uuid id, std::unordered_map<Uuid, T>& map, std::optional<T>& fallback, ImporterFunc importFunc)
    {
      auto it = map.find(id);
      if (it != map.end())
      {
        return it->second;
      }

      const Engine::AssetRecord& record = m_AssetRegistry.GetRecord(id);
      if (!record.IsValid())
      {
        LOG_ERROR("Failed to load asset '{}', Record not found", id);
        return *fallback;
      }

      auto path = m_AssetsDirectory / record.path;
      std::optional<T> importedAsset = importFunc(path);

      if (importedAsset)
      {
        importedAsset->id = id;
        auto [insertedIt, success] = map.insert_or_assign(id, std::move(*importedAsset));
        LOG_TRACE("Loaded asset '{}'", id);
        return insertedIt->second;
      }

      LOG_ERROR("Asset import failed! '{}'", id);
      return *fallback;
    }
  }

  void AssetManager::Initialize(const std::filesystem::path& projectDirectory)
  {
    m_AssetRegistry.Load(projectDirectory / "assetRegistry.yaml");
		m_AssetsDirectory = projectDirectory / "Assets";

		s_defaultTexture = *Texture2D::GetDefault();
    s_defaultMesh.emplace(MeshSpecification{});
		s_defaultShader.emplace(ShaderSpecification{}, "", "EmptyShader");
    s_defaultMaterial = *Materials::GetDefault();
		s_defaultAudioClip = AudioClip();
    s_defaultScene = Scene();

		auto cubeMesh = MeshImporter::LoadMesh("Resources/Engine/models/cube.glb");
    if(cubeMesh)
    {
      cubeMesh->id = RESOURCES::MESH::CUBE;
			m_Meshes.insert_or_assign(RESOURCES::MESH::CUBE, std::move(*cubeMesh));
		}

		auto sphereMesh = MeshImporter::LoadMesh("Resources/Engine/models/sphere.glb");
    if(sphereMesh)
    {
      sphereMesh->id = RESOURCES::MESH::SPHERE;
      m_Meshes.insert_or_assign(RESOURCES::MESH::SPHERE, std::move(*sphereMesh));
		}

		auto HDRTexture = TextureImporter::LoadTexture("Resources/Engine/hdr/PG2/lebombo_4k.hdr");
    if(HDRTexture)
    {
      HDRTexture->id = RESOURCES::TEXTURE::HDR;
      m_Textures.insert_or_assign(RESOURCES::TEXTURE::HDR, std::move(*HDRTexture));
    }
  }

  AssetRegistry& AssetManager::GetAssetRegistry()
  {
		return m_AssetRegistry;
  }

  template<> Texture2D& AssetManager::GetAsset<Texture2D>(Uuid id)
  {
    return LoadAssetInternal(id, m_Textures, s_defaultTexture, TextureImporter::LoadTexture);
  }

  template<> Mesh& AssetManager::GetAsset<Mesh>(Uuid id)
  {
    return LoadAssetInternal(id, m_Meshes, s_defaultMesh, MeshImporter::LoadMesh);
  }

  template<> Shader& AssetManager::GetAsset<Shader>(Uuid id)
  {
    return LoadAssetInternal(id, m_Shaders, s_defaultShader, ShaderImporter::LoadShader);
  }

  template<> Material& AssetManager::GetAsset<Material>(Uuid id)
  {
    return LoadAssetInternal(id, m_Materials, s_defaultMaterial, MaterialImporter::LoadMaterial);
  }

  template<> AudioClip& AssetManager::GetAsset<AudioClip>(Uuid id)
  {
    return LoadAssetInternal(id, m_AudioClips, s_defaultAudioClip, AudioImporter::LoadAudioClip);
  }

  template<> Scene& AssetManager::GetAsset<Scene>(Uuid id)
  {
    return LoadAssetInternal(id, m_Scenes, s_defaultScene, SceneImporter::LoadScene);
  }
}