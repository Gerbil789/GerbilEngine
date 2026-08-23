#include "enginepch.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Asset/Resources.h"

#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Importer/AudioImporter.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Core/Scene.h"
#include "Engine/Audio/AudioClip.h"

#include "Engine/Event/EventBus.h"
#include "Engine/Event/FileEvent.h"

namespace Engine
{
  namespace
  {
    AssetRegistry m_AssetRegistry;
    std::filesystem::path m_AssetsDirectory;

    std::unordered_map<Texture2D, Texture2DAsset> m_Textures;
    std::unordered_map<Mesh, MeshAsset> m_Meshes;
    std::unordered_map<Shader, ShaderAsset> m_Shaders;
    std::unordered_map<Material, MaterialAsset> m_Materials;
    std::unordered_map<AudioClip, AudioClipAsset> m_AudioClips;
    std::unordered_map<Scene, SceneAsset> m_Scenes;

    std::vector<Uuid> m_DirtySet;

    template<typename Handle, typename Asset, typename ImporterFunc>
    Asset& GetOrImport(Handle handle, Handle fallback, std::unordered_map<Handle, Asset>& map, ImporterFunc importer)
    {
      if (auto it = map.find(handle); it != map.end()) return it->second;

      const Engine::AssetRecord& record = m_AssetRegistry.GetRecord(handle.id);
      if (!record) return map.at(fallback);

      std::optional<Asset> importedAsset = importer(m_AssetsDirectory / record.path);
      if (!importedAsset) return map.at(fallback);

      importedAsset->id = handle.id;
      auto [it, ok] = map.try_emplace(handle, std::move(*importedAsset));
      return it->second;
    }

    template<typename Handle, typename Asset, typename ImporterFunc>
    void ImportBuiltIn(Handle handle, const std::string& path, std::unordered_map<Handle, Asset>& map, ImporterFunc importer)
    {
      std::optional<Asset> asset = importer(path);
      if (asset)
      {
        asset->id = handle.id;
        map.try_emplace(handle, std::move(*asset));
      }
      else
      {
        LOG_ERROR("Failed to load built-in asset: {}", path);
      }
    }

    template<typename Handle>
    void LoadBuiltInAsset(Handle handle, const std::string& path)
    {
      if constexpr (std::is_same_v<Handle, Texture2D>) ImportBuiltIn(handle, path, m_Textures, TextureImporter::LoadTexture2D);
      else if constexpr (std::is_same_v<Handle, Mesh>) ImportBuiltIn(handle, path, m_Meshes, MeshImporter::LoadMesh);
      else if constexpr (std::is_same_v<Handle, Shader>) ImportBuiltIn(handle, path, m_Shaders, ShaderImporter::LoadShader);
      else if constexpr (std::is_same_v<Handle, Material>) ImportBuiltIn(handle, path, m_Materials, MaterialSerializer::Deserialize);
      else if constexpr (std::is_same_v<Handle, AudioClip>) ImportBuiltIn(handle, path, m_AudioClips, AudioImporter::LoadAudioClip);
      else if constexpr (std::is_same_v<Handle, Scene>) ImportBuiltIn(handle, path, m_Scenes, SceneImporter::LoadScene);
    }
  }
 

  void AssetManager::Initialize(const std::filesystem::path& projectDirectory)
  {
    m_AssetRegistry.Load();
		m_AssetsDirectory = projectDirectory / "Assets";

    Engine::EventBus::Subscribe<Engine::FileAddedEvent>([](const Engine::FileAddedEvent& event)
      {
        LOG_WARNING("File added event received");
        m_AssetRegistry.AddRecord(Uuid::Generate(), event.path);
        return false;
      });

    Engine::EventBus::Subscribe<Engine::FileRemovedEvent>([](const Engine::FileRemovedEvent& event)
      {
        LOG_WARNING("File removed event received");

        Engine::Uuid id = m_AssetRegistry.GetIdFromPath(event.path);
				m_AssetRegistry.RemoveRecord(id);
        return false;
      });


    auto emptyMesh = MeshAsset(MeshSpecification{});
    emptyMesh.id = Uuid{ RESOURCES::MESH::EMPTY };
    m_Meshes.insert_or_assign(RESOURCES::MESH::EMPTY, std::move(emptyMesh));

    {
      TextureSpecification spec;
      constexpr uint32_t whitePixel = 0xFFFFFFFF;
      Texture2DAsset whiteTexture(spec, &whitePixel);
      whiteTexture.id = Uuid{ RESOURCES::TEXTURE::WHITE };
      m_Textures.insert_or_assign(RESOURCES::TEXTURE::WHITE, std::move(whiteTexture));
    }

    {
      TextureSpecification spec;
      constexpr uint8_t normalPixel[4] = { 128, 128, 255, 255 }; // Blue-ish
      Texture2DAsset normalTexture(spec, &normalPixel);
      normalTexture.id = Uuid{ RESOURCES::TEXTURE::NORMAL };
      m_Textures.insert_or_assign(RESOURCES::TEXTURE::NORMAL, std::move(normalTexture));
    }

    LoadBuiltInAsset(RESOURCES::MESH::CUBE, "resources/models/cube.glb");
    LoadBuiltInAsset(RESOURCES::MESH::SPHERE, "resources/models/sphere.glb");
    LoadBuiltInAsset(RESOURCES::TEXTURE::HDR, "resources/hdr/lebombo_4k.hdr");
    LoadBuiltInAsset(RESOURCES::TEXTURE::EDITOR_ICONS, "resources/icons/icons.png");
    LoadBuiltInAsset(RESOURCES::SHADER::DEFAULT, "resources/shaders/pink.wgsl");
    LoadBuiltInAsset(RESOURCES::SHADER::FLAT, "resources/shaders/flat.wgsl");
    LoadBuiltInAsset(RESOURCES::SHADER::UI, "resources/shaders/ui.wgsl");
    LoadBuiltInAsset(RESOURCES::SCENE::DEFAULT, "resources/scenes/default.scene");
    LoadBuiltInAsset(RESOURCES::TEXTURE::DEFAULT_FONT_ATLAS, "resources/fonts/atlas.png");

    {
      MaterialSpecification spec{ RESOURCES::SHADER::FLAT };
      MaterialAsset whiteMaterial = MaterialAsset(spec);
      whiteMaterial.SetParameter("albedo", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
      whiteMaterial.id = Uuid{ RESOURCES::MATERIAL::WHITE };
      m_Materials.insert_or_assign(RESOURCES::MATERIAL::WHITE, std::move(whiteMaterial));
    }

    {
      MaterialSpecification spec{ RESOURCES::SHADER::DEFAULT };
      MaterialAsset pinkMaterial = MaterialAsset(spec);
      pinkMaterial.id = Uuid{ RESOURCES::MATERIAL::PINK };
      m_Materials.insert_or_assign(RESOURCES::MATERIAL::PINK, std::move(pinkMaterial));
    }
  }

  AssetRegistry& AssetManager::GetAssetRegistry()
  {
		return m_AssetRegistry;
  }

  bool AssetManager::Exists(Uuid id)
  {
    return m_AssetRegistry.Exists(id);
  }

  template<typename Asset>
  Asset& AssetManager::GetAsset(AssetHandle<Asset> handle)
  {
    if constexpr (std::is_same_v<Asset, Texture2DAsset>) return GetOrImport(handle, RESOURCES::TEXTURE::WHITE, m_Textures, TextureImporter::LoadTexture2D);
    else if constexpr (std::is_same_v<Asset, MeshAsset>) return GetOrImport(handle, RESOURCES::MESH::EMPTY, m_Meshes, MeshImporter::LoadMesh);
    else if constexpr (std::is_same_v<Asset, ShaderAsset>) return GetOrImport(handle, RESOURCES::SHADER::FLAT, m_Shaders, ShaderImporter::LoadShader);
    else if constexpr (std::is_same_v<Asset, MaterialAsset>) return GetOrImport(handle, RESOURCES::MATERIAL::WHITE, m_Materials, MaterialSerializer::Deserialize);
    else if constexpr (std::is_same_v<Asset, AudioClipAsset>) return GetOrImport(handle, AudioClip{}, m_AudioClips, AudioImporter::LoadAudioClip);
    else if constexpr (std::is_same_v<Asset, SceneAsset>) return GetOrImport(handle, RESOURCES::SCENE::DEFAULT, m_Scenes, SceneImporter::LoadScene);
    else static_assert(false, "Unsupported asset type requested in GetAsset!");
  }

  template Texture2DAsset& AssetManager::GetAsset(Texture2D texture);
  template MeshAsset& AssetManager::GetAsset(Mesh mesh);
  template ShaderAsset& AssetManager::GetAsset(Shader shader);
  template MaterialAsset& AssetManager::GetAsset(Material material);
  template AudioClipAsset& AssetManager::GetAsset(AudioClip clip);
  template SceneAsset& AssetManager::GetAsset(Scene scene);

  AssetType AssetManager::GetAssetType(Uuid id)
  {
    return m_AssetRegistry.GetType(id);
  }

  const std::filesystem::path& AssetManager::GetAssetPath(Uuid id)
  {
		return m_AssetRegistry.GetRecord(id).path;
  }

  template<typename Handle>
  Handle AssetManager::CreateAsset(const std::filesystem::path& path)
  {
    if constexpr (std::is_same_v<Handle, Material>)
    {
      Material handle{ Uuid::Generate() };

      MaterialAsset material(MaterialSpecification{});
      material.id = handle.id;

      m_Materials.insert_or_assign(handle, std::move(material));

      m_AssetRegistry.AddRecord(handle.id, path);
      MaterialSerializer::Serialize(handle, path);

      LOG_TRACE("Created material asset '{}'", handle.id);

      return handle;
    }
    else if constexpr (std::is_same_v<Handle, Scene>)
    {
      Scene handle{ Uuid::Generate() };

      SceneAsset scene;
      scene.id = handle.id;

      m_Scenes.insert_or_assign(handle, std::move(scene));

      m_AssetRegistry.AddRecord(handle.id, path);
      SceneSerializer::Serialize(handle, path);

      LOG_TRACE("Created scene asset '{}'", handle.id);
      return handle;
    }
  }

	template Material AssetManager::CreateAsset(const std::filesystem::path& path);
	template Scene AssetManager::CreateAsset(const std::filesystem::path& path);



  void AssetManager::MarkAssetDirty(Uuid id)
  {
    if (Exists(id))
    {
      m_DirtySet.push_back(id);
    }
  }

  void AssetManager::SaveDirtyAssets()
  {
    for (Uuid id : m_DirtySet)
    {
      const auto& record = m_AssetRegistry.GetRecord(id);

      switch (record.type)
      {
      case Engine::AssetType::Material:
      {
        Engine::MaterialSerializer::Serialize(Material{ record.id }, record.path);
        break;
      }
      default:
      {

      }
      }
    }

    m_DirtySet.clear();
  }
}