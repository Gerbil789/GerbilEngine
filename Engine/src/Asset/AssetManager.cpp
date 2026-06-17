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
#include "Engine/Graphics/Camera.h"

#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/MaterialImporter.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Importer/AudioImporter.h"

#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

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

    template<typename T, typename ImporterFunc>
    T& LoadAssetInternal(Uuid id, std::unordered_map<Uuid, T>& map, Uuid fallbackId, ImporterFunc importFunc)
    {
      if (auto it = map.find(id); it != map.end())
      {
        return it->second;
      }

      const Engine::AssetRecord& record = m_AssetRegistry.GetRecord(id);
      if (!record) return map.at(fallbackId);

      auto path = m_AssetsDirectory / record.path;
      std::optional<T> importedAsset = importFunc(path);

      if (importedAsset)
      {
        importedAsset->id = id;

        auto [it, ok] = map.try_emplace(id, std::move(*importedAsset));
        LOG_TRACE("Loaded asset '{}'", id);
        return it->second;
      }

      LOG_ERROR("Asset import failed! '{}'", id);
      return map.at(fallbackId);
    }

    template<typename T, typename ImporterFunc>
    void LoadBuiltInAsset(Uuid id, const std::string& filepath, std::unordered_map<Uuid, T>& map, ImporterFunc importFunc, const char* = nullptr)
    {
      auto importedAsset = importFunc(filepath);
      if (importedAsset)
      {
        importedAsset->id = id;

        map.insert_or_assign(id, std::move(*importedAsset));
        LOG_TRACE("Loaded built-in asset '{}' from '{}'", id, filepath);
      }
      else
      {
        LOG_ERROR("Failed to load built-in asset from '{}'", filepath);
      }
    }
  }

  void AssetManager::Initialize(const std::filesystem::path& projectDirectory)
  {
    m_AssetRegistry.Load(projectDirectory / "assetRegistry.json");
		m_AssetsDirectory = projectDirectory / "Assets";

    auto emptyMesh = Mesh(MeshSpecification{});
    emptyMesh.id = RESOURCES::MESH::EMPTY;
    m_Meshes.insert_or_assign(RESOURCES::MESH::EMPTY, std::move(emptyMesh));

    {
      TextureSpecification spec;
      constexpr uint32_t whitePixel = 0xFFFFFFFF;
      Texture2D whiteTexture(spec, &whitePixel);
      whiteTexture.id = RESOURCES::TEXTURE::WHITE;
      m_Textures.insert_or_assign(RESOURCES::TEXTURE::WHITE, std::move(whiteTexture));
    }

    {
      TextureSpecification spec;
      constexpr uint8_t normalPixel[4] = { 128, 128, 255, 255 }; // Blue-ish
      Texture2D normalTexture(spec, &normalPixel);
      normalTexture.id = RESOURCES::TEXTURE::NORMAL;
      m_Textures.insert_or_assign(RESOURCES::TEXTURE::NORMAL, std::move(normalTexture));
    }

    LoadBuiltInAsset(RESOURCES::MESH::CUBE, "Resources/Engine/models/cube.glb", m_Meshes, MeshImporter::LoadMesh, "Cube");
    LoadBuiltInAsset(RESOURCES::MESH::SPHERE, "Resources/Engine/models/sphere.glb", m_Meshes, MeshImporter::LoadMesh, "Sphere");
    LoadBuiltInAsset(RESOURCES::TEXTURE::HDR, "Resources/Engine/hdr/lebombo_4k.hdr", m_Textures, TextureImporter::LoadTexture2D, "HDR Environment");
    LoadBuiltInAsset(RESOURCES::TEXTURE::EDITOR_ICONS, "Resources/Engine/icons/icons.png", m_Textures, TextureImporter::LoadTexture2D, "Editor Icons");
    LoadBuiltInAsset(RESOURCES::SHADER::DEFAULT, "Resources/Engine/shaders/pink.wgsl", m_Shaders, ShaderImporter::LoadShader, "Pink Shader");
    LoadBuiltInAsset(RESOURCES::SHADER::FLAT, "Resources/Engine/shaders/flat.wgsl", m_Shaders, ShaderImporter::LoadShader, "Flat Shader");
    LoadBuiltInAsset(RESOURCES::SCENE::DEFAULT, "Resources/Engine/scenes/default.scene", m_Scenes, SceneImporter::LoadScene, "Default Scene");

    {
      MaterialSpecification spec{ .shaderId = RESOURCES::SHADER::FLAT };
      Material whiteMaterial = Material(spec);
      whiteMaterial.SetParameter("albedo", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
      whiteMaterial.id = RESOURCES::MATERIAL::WHITE;
      m_Materials.insert_or_assign(RESOURCES::MATERIAL::WHITE, std::move(whiteMaterial));
    }

    {
      MaterialSpecification spec{ .shaderId = RESOURCES::SHADER::DEFAULT };
      Material pinkMaterial = Material(spec);
      pinkMaterial.id = RESOURCES::MATERIAL::PINK;
      m_Materials.insert_or_assign(RESOURCES::MATERIAL::PINK, std::move(pinkMaterial));
    }
  }

  AssetRegistry& AssetManager::GetAssetRegistry()
  {
		return m_AssetRegistry;
  }

  template<typename T>
  T& AssetManager::GetAsset(Uuid id)
  {
    if constexpr (std::is_same_v<T, Texture2D>)
    {
      return LoadAssetInternal(id, m_Textures, RESOURCES::TEXTURE::WHITE, TextureImporter::LoadTexture2D);
    }
    else if constexpr (std::is_same_v<T, Mesh>)
    {
      return LoadAssetInternal(id, m_Meshes, RESOURCES::MESH::EMPTY, MeshImporter::LoadMesh);
    }
    else if constexpr (std::is_same_v<T, Shader>)
    {
      return LoadAssetInternal(id, m_Shaders, RESOURCES::SHADER::FLAT, ShaderImporter::LoadShader);
    }
    else if constexpr (std::is_same_v<T, Material>)
    {
      return LoadAssetInternal(id, m_Materials, RESOURCES::MATERIAL::WHITE, MaterialImporter::LoadMaterial);
    }
    else if constexpr (std::is_same_v<T, AudioClip>)
    {
      return LoadAssetInternal(id, m_AudioClips, Uuid{}, AudioImporter::LoadAudioClip);
    }
    else if constexpr (std::is_same_v<T, Scene>)
    {
      return LoadAssetInternal(id, m_Scenes, RESOURCES::SCENE::DEFAULT, SceneImporter::LoadScene);
    }
    else
    {
      static_assert(false, "ERROR: Unsupported asset type requested in GetAsset!");
    }
  }

  template ENGINE_API Texture2D& AssetManager::GetAsset<Texture2D>(Uuid id);
  template ENGINE_API Mesh& AssetManager::GetAsset<Mesh>(Uuid id);
  template ENGINE_API Shader& AssetManager::GetAsset<Shader>(Uuid id);
  template ENGINE_API Material& AssetManager::GetAsset<Material>(Uuid id);
  template ENGINE_API AudioClip& AssetManager::GetAsset<AudioClip>(Uuid id);
  template ENGINE_API Scene& AssetManager::GetAsset<Scene>(Uuid id);

  template<typename T>
  T& AssetManager::CreateAsset<T>(const std::filesystem::path& path)
  {
    if constexpr (std::is_same_v<T, Material>)
    {
      MaterialSpecification spec
      {
        .shaderId = RESOURCES::SHADER::DEFAULT,
      };

      Material material(spec);
      material.id = Uuid::Generate();

      LOG_TRACE("Created asset '{}'", material.id);
      auto [insertedIt, success] = m_Materials.insert_or_assign(material.id, std::move(material));


      m_AssetRegistry.Create(path); // save record in assetRegistry.json
      MaterialSerializer::Serialize(insertedIt->second, path); // immediately serialize to create .mat file

      return insertedIt->second;
    }
    else if constexpr (std::is_same_v<T, Scene>)
    {
      Scene scene;
      scene.id = Uuid::Generate();

      LOG_TRACE("Created asset '{}'", scene.id);
      auto [insertedIt, success] = m_Scenes.insert_or_assign(scene.id, std::move(scene));

      m_AssetRegistry.Create(path); // save record in assetRegistry.json
      SceneSerializer::Serialize(insertedIt->second, path); // immediately serialize to create .scene file


      return insertedIt->second;
    }
  }

	template ENGINE_API Material& AssetManager::CreateAsset<Material>(const std::filesystem::path& path);
	template ENGINE_API Scene& AssetManager::CreateAsset<Scene>(const std::filesystem::path& path);
}