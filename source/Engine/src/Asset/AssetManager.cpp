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
#include "Engine/Graphics/Font.h"

#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Importer/AudioImporter.h"

#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

#include "Engine/Event/EventBus.h"
#include "Engine/Event/FileEvent.h"

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

    std::vector<Uuid> m_DirtySet;

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

    LoadBuiltInAsset(RESOURCES::MESH::CUBE, "resources/models/cube.glb", m_Meshes, MeshImporter::LoadMesh, "Cube");
    LoadBuiltInAsset(RESOURCES::MESH::SPHERE, "resources/models/sphere.glb", m_Meshes, MeshImporter::LoadMesh, "Sphere");
    LoadBuiltInAsset(RESOURCES::TEXTURE::HDR, "resources/hdr/lebombo_4k.hdr", m_Textures, TextureImporter::LoadTexture2D, "HDR Environment");
    LoadBuiltInAsset(RESOURCES::TEXTURE::EDITOR_ICONS, "resources/icons/icons.png", m_Textures, TextureImporter::LoadTexture2D, "Editor Icons");
    LoadBuiltInAsset(RESOURCES::SHADER::DEFAULT, "resources/shaders/pink.wgsl", m_Shaders, ShaderImporter::LoadShader, "Pink Shader");
    LoadBuiltInAsset(RESOURCES::SHADER::FLAT, "resources/shaders/flat.wgsl", m_Shaders, ShaderImporter::LoadShader, "Flat Shader");
    LoadBuiltInAsset(RESOURCES::SHADER::UI, "resources/shaders/ui.wgsl", m_Shaders, ShaderImporter::LoadShader, "UI Shader");
    LoadBuiltInAsset(RESOURCES::SCENE::DEFAULT, "resources/scenes/default.scene", m_Scenes, SceneImporter::LoadScene, "Default Scene");
    LoadBuiltInAsset(RESOURCES::TEXTURE::DEFAULT_FONT_ATLAS, "resources/fonts/atlas.png", m_Textures, TextureImporter::LoadTexture2D, "Default Font Atlas");

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

  bool AssetManager::Exists(Uuid id)
  {
    return m_AssetRegistry.Exists(id);
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
      return LoadAssetInternal(id, m_Materials, RESOURCES::MATERIAL::WHITE, MaterialSerializer::Deserialize);
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

  AssetType AssetManager::GetAssetType(Uuid id)
  {
    return m_AssetRegistry.GetType(id);
  }

  const std::filesystem::path& AssetManager::GetAssetPath(Uuid id)
  {
		return m_AssetRegistry.GetRecord(id).path;
  }

  std::vector<Uuid> AssetManager::GetAssetsOfType(AssetType type)
  {
    switch(type)
    {
    case AssetType::Texture: return m_Textures | std::views::keys | std::ranges::to<std::vector<Uuid>>(); break;
		case AssetType::Mesh: return m_Meshes | std::views::keys | std::ranges::to<std::vector<Uuid>>(); break;
		case AssetType::Shader: return m_Shaders | std::views::keys | std::ranges::to<std::vector<Uuid>>(); break;
		case AssetType::Material: return m_Materials | std::views::keys | std::ranges::to<std::vector<Uuid>>(); break;
		case AssetType::Audio: return m_AudioClips | std::views::keys | std::ranges::to<std::vector<Uuid>>(); break;
		case AssetType::Scene: return m_Scenes | std::views::keys | std::ranges::to<std::vector<Uuid>>(); break;
		default: return {}; break;
    }
  }

  template<typename T>
  T& AssetManager::CreateAsset<T>(const std::filesystem::path& path)
  {
    if constexpr (std::is_same_v<T, Material>)
    {
      Uuid id = Uuid::Generate();

      MaterialSpecification spec
      {
        .shaderId = RESOURCES::SHADER::DEFAULT,
      };

      Material material(spec);
      material.id = id;

      auto [insertedIt, success] = m_Materials.insert_or_assign(material.id, std::move(material));

      m_AssetRegistry.AddRecord(id, path); // save record in assetRegistry.json
      MaterialSerializer::Serialize(id, path); // immediately serialize to create .mat file

      LOG_TRACE("Created material asset '{}'", id);

      return insertedIt->second;
    }
    else if constexpr (std::is_same_v<T, Scene>)
    {
      Uuid id = Uuid::Generate();

      Scene scene;
      scene.id = id;

      auto [insertedIt, success] = m_Scenes.insert_or_assign(id, std::move(scene));

      m_AssetRegistry.AddRecord(id, path); // save record in assetRegistry.json
      SceneSerializer::Serialize(id, path); // immediately serialize to create .scene file

      LOG_TRACE("Created scene asset '{}'", id);
      return insertedIt->second;
    }
  }

	template ENGINE_API Material& AssetManager::CreateAsset<Material>(const std::filesystem::path& path);
	template ENGINE_API Scene& AssetManager::CreateAsset<Scene>(const std::filesystem::path& path);



  void AssetManager::MarkAssetDirty(Uuid id)
  {
    if (Exists(id))
    {
      m_DirtySet.push_back(id);
    }
  }

  void AssetManager::SaveDirtyAssets()
  {
    for (const auto& id : m_DirtySet)
    {
      const auto& record = m_AssetRegistry.GetRecord(id);

      switch (record.type)
      {
      case Engine::AssetType::Material:
      {
        Engine::MaterialSerializer::Serialize(record.id, record.path);
        break;
      }
      }
    }

    m_DirtySet.clear();
  }
}