#include "enginepch.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Asset/Importer/SceneImporter.h"
#include "Engine/Asset/Importer/AudioImporter.h"
#include "Engine/Asset/Serializer/Serializer.h"

namespace engine
{
  void AssetManager::Initialize()
  {
    auto addBuiltIn = [&]<typename Handle, typename... Args>(Handle handle, const std::string& nameOrPath, Args&&... args)
    {
      AssetRegistry::AddRecord(handle, nameOrPath);
      
      auto& assetMap = std::get<AssetDataMap<Handle>>(m_LoadedAssets);
      assetMap.try_emplace(handle, std::forward<Args>(args)...);
    };

    addBuiltIn(TextureHandle::White, "White Texture", TextureSpecification{}, std::array<uint32_t, 1>{0xFFFFFFFF}.data());
    addBuiltIn(TextureHandle::Normal, "Normal Texture", TextureSpecification{}, std::array<uint32_t, 1>{0xFFFF8080}.data());
    addBuiltIn(TextureHandle::HDR, "resources/hdr/autumn_field_puresky_4k.hdr", TextureImporter::LoadTexture2D("resources/hdr/autumn_field_puresky_4k.hdr"));
    addBuiltIn(TextureHandle::EDITOR_ICONS, "resources/icons/icons.png", TextureImporter::LoadTexture2D("resources/icons/icons.png"));
    addBuiltIn(TextureHandle::DEFAULT_FONT_ATLAS, "resources/fonts/atlas.png", TextureImporter::LoadTexture2D("resources/fonts/atlas.png"));
    addBuiltIn(TextureHandle::EDITOR_THUMBNAIL_ATLAS, "Editor Thumbnail Atlas", TextureSpecification{ .width = 2048, .height = 2048, .format = engine::GraphicsContext::GetSurfaceFormat() }, nullptr);

    addBuiltIn(MeshHandle::Empty, "Empty Mesh", MeshImporter::LoadMesh("resources/models/cube.glb"));
    addBuiltIn(MeshHandle::Cube, "resources/models/cube.glb", MeshImporter::LoadMesh("resources/models/cube.glb"));
    addBuiltIn(MeshHandle::Sphere, "resources/models/sphere.glb", MeshImporter::LoadMesh("resources/models/sphere.glb"));

    addBuiltIn(ShaderHandle::Pink, "resources/shaders/pink.wgsl", ShaderImporter::LoadShader("resources/shaders/pink.wgsl"));
    addBuiltIn(ShaderHandle::Flat, "resources/shaders/flat.wgsl", ShaderImporter::LoadShader("resources/shaders/flat.wgsl"));
    addBuiltIn(ShaderHandle::UI, "resources/shaders/ui.wgsl", ShaderImporter::LoadShader("resources/shaders/ui.wgsl"));
    addBuiltIn(ShaderHandle::Skybox, "resources/shaders/skybox.wgsl", ShaderImporter::LoadShader("resources/shaders/skybox.wgsl"));
    addBuiltIn(ShaderHandle::Normal, "resources/shaders/normal.wgsl", ShaderImporter::LoadShader("resources/shaders/normal.wgsl"));

    addBuiltIn(SceneHandle::Default, "resources/scenes/default.scene", SceneImporter::LoadScene("resources/scenes/default.scene"));
    
    addBuiltIn(MaterialHandle::Pink, "Pink Material", Material(ShaderHandle::Pink));
    addBuiltIn(MaterialHandle::Pink, "White Material", Material(ShaderHandle::Flat));
    
    addBuiltIn(MaterialInstanceHandle::Pink, "Pink Material Instance", MaterialInstance(MaterialHandle::Pink));
    addBuiltIn(MaterialInstanceHandle::Pink, "White Material Instance", MaterialInstance(MaterialHandle::White)); //TODO: white material is not working, it fallbacks to the pink

    addBuiltIn(AudioClipHandle::null, "Empty Audio Clip", AudioClip{});

    auto& textures = std::get<AssetDataMap<TextureHandle>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<TextureHandle>())
    {
      textures.try_emplace(handle, TextureImporter::LoadTexture2D(path));
    }

    auto& meshes = std::get<AssetDataMap<MeshHandle>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<MeshHandle>())
    {
      meshes.try_emplace(handle, MeshImporter::LoadMesh(path));
    }

    auto& shaders = std::get<AssetDataMap<ShaderHandle>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<ShaderHandle>())
    {
      shaders.try_emplace(handle, ShaderImporter::LoadShader(path.string()));
    }

    auto& scenes = std::get<AssetDataMap<SceneHandle>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<SceneHandle>())
    {
      scenes.try_emplace(handle, SceneImporter::LoadScene(path));
    }

    auto& materials = std::get<AssetDataMap<MaterialHandle>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<MaterialHandle>())
    {
      materials.try_emplace(handle, Serializer::DeserializeMaterial(path));
    }

    auto& materialInstances = std::get<AssetDataMap<MaterialInstanceHandle>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<MaterialInstanceHandle>())
    {
      materialInstances.try_emplace(handle, Serializer::DeserializeMaterialInstance(path));
    }

    auto& audioClips = std::get<AssetDataMap<AudioClipHandle>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<AudioClipHandle>())
    {
      audioClips.try_emplace(handle, AudioImporter::LoadAudioClip(path));
    }
  }

  // template <typename Handle>
  // Handle AssetManager::CreateAsset(const std::filesystem::path& path)
  // {
  //   // if constexpr (std::is_same_v<Handle, Material>)
  //   // {
  //   //   Material material{ Uuid::Generate() };
  //   //   MaterialAsset data(MaterialSpecification{});

  //   //   m_Materials.insert_or_assign(material, std::move(data));

  //   //   m_AssetRegistry.AddRecord(material.id, path);
  //   //   MaterialSerializer::Serialize(material, Project::AssetsDirectory() / path);

  //   //   LOG_TRACE("Created new material: {}, {}", path.stem(), material.id);

  //   //   return material;
  //   // }
  //   // else 
  //   // if constexpr (std::is_same_v<Handle, Scene>)
  //   // {
  //   //   Scene scene{ Uuid::Generate() };
  //   //   SceneAsset data;

  //   //   m_Scenes.insert_or_assign(scene, std::move(data));

  //   //   AssetRegistry::AddRecord(scene, path);
  //   //   Serializer::SerializeScene(scene, path);

  //   //   LOG_TRACE("Created new scene: {}, {}", path.stem(), scene.id);
  //   //   return scene;
  //   // }
  // }

  //template Material AssetManager::CreateAsset(const std::filesystem::path& path);
  //template Scene AssetManager::CreateAsset(const std::filesystem::path& path);

  void AssetManager::SaveDirtyAssets()
  {
    auto saveSet = []<typename Handle>(AssetSet<Handle>&dirtySet)
    {
      for (Handle handle : dirtySet)
      {
        std::filesystem::path path = AssetRegistry::GetAssetPath(handle);

        if constexpr (std::is_same_v<Handle, SceneHandle>)
        {
          Serializer::SerializeScene(handle, path);
        }
        else if constexpr (std::is_same_v<Handle, MaterialHandle>)
        {
          Serializer::SerializeMaterial(handle, path);
        }

        LOG_TRACE("Saved asset to {}", path);
      }

      dirtySet.clear();
    };

    std::apply([&](auto&... sets) { (saveSet(sets), ...); }, m_DirtyAssets);
  }
}