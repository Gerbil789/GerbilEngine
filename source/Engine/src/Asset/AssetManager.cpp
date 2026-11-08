#include "enginepch.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Serializer/Serializer.h"

namespace engine
{
  void AssetManager::Initialize()
  {
    auto addBuiltIn = [&]<typename T, typename... Args>(Handle<T> handle, const std::string& path, Args&&... args)
    {
      AssetRegistry::AddRecord(handle, path);
      
      auto& assetMap = std::get<AssetDataMap<T>>(m_loadedAssets);
      assetMap.try_emplace(handle, std::forward<Args>(args)...);
    };

    addBuiltIn(texture::White, "White Texture", Texture2D::Create({}, std::array<uint32_t, 1>{0xFFFFFFFF}.data()));
    addBuiltIn(texture::Normal, "Normal Texture", Texture2D::Create({}, std::array<uint32_t, 1>{0xFFFF8080}.data()));
    addBuiltIn(texture::HDR, "resources/hdr/autumn_field_puresky_4k.hdr", Texture2D::Create("resources/hdr/autumn_field_puresky_4k.hdr"));
    addBuiltIn(texture::EditorIcons, "resources/icons/icons.png", Texture2D::Create("resources/icons/icons.png"));
    addBuiltIn(texture::DefaultFontAtlas, "resources/fonts/atlas.png", Texture2D::Create("resources/fonts/atlas.png"));
    addBuiltIn(texture::EditorThumbnailAtlas, "Editor Thumbnail Atlas", Texture2D::Create({ .width = 2048, .height = 2048, .format = engine::GraphicsContext::GetSurfaceFormat() }, nullptr));

    addBuiltIn(mesh::Empty, "Empty Mesh", Mesh{});
    addBuiltIn(mesh::Cube, "resources/models/cube.glb", Mesh::Create("resources/models/cube.glb"));
    addBuiltIn(mesh::Sphere, "resources/models/sphere.glb", Mesh::Create("resources/models/sphere.glb"));

    addBuiltIn(shader::Pink, "resources/shaders/pink.wgsl", Shader::Create("resources/shaders/pink.wgsl"));
    addBuiltIn(shader::Flat, "resources/shaders/flat.wgsl", Shader::Create("resources/shaders/flat.wgsl"));
    addBuiltIn(shader::UI, "resources/shaders/ui.wgsl", Shader::Create("resources/shaders/ui.wgsl"));
    addBuiltIn(shader::Skybox, "resources/shaders/skybox.wgsl", Shader::Create("resources/shaders/skybox.wgsl"));
    addBuiltIn(shader::Normal, "resources/shaders/normal.wgsl", Shader::Create("resources/shaders/normal.wgsl"));

    addBuiltIn(scene::Empty, "Empty Scene", Scene{});
    addBuiltIn(scene::Default, "resources/scenes/default.scene", Serializer::DeserializeScene("resources/scenes/default.scene"));
    
    addBuiltIn(material::Pink, "Pink Material", Material{shader::Pink});
    addBuiltIn(material::Pink, "White Material", Material{shader::Flat});
    
    addBuiltIn(material_instance::Pink, "Pink Material Instance", MaterialInstance{material::Pink});
    addBuiltIn(material_instance::Pink, "White Material Instance", MaterialInstance{material::White}); //TODO: white material is not working, it fallbacks to the pink

    addBuiltIn(audio::Empty, "Empty Audio Clip", AudioClip{});

    auto& textures = std::get<AssetDataMap<Texture2D>>(m_loadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Texture2D>())
    {
      if(static_cast<uint64_t>(handle.id) < 1000) continue;
      textures.try_emplace(handle, Texture2D::Create(path));
    }

    auto& meshes = std::get<AssetDataMap<Mesh>>(m_loadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Mesh>())
    {
      if(static_cast<uint64_t>(handle.id) < 1000) continue;
      meshes.try_emplace(handle, Mesh::Create(path));
    }

    auto& shaders = std::get<AssetDataMap<Shader>>(m_loadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Shader>())
    {
      if(static_cast<uint64_t>(handle.id) < 1000) continue;
      shaders.try_emplace(handle, Shader::Create(path));
    }

    auto& scenes = std::get<AssetDataMap<Scene>>(m_loadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Scene>())
    {
      if(static_cast<uint64_t>(handle.id) < 1000) continue;
      scenes.try_emplace(handle, Serializer::DeserializeScene(path));
    }

    auto& materials = std::get<AssetDataMap<Material>>(m_loadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Material>())
    {
      if(static_cast<uint64_t>(handle.id) < 1000) continue;
      materials.try_emplace(handle, Serializer::DeserializeMaterial(path));
    }

    auto& materialInstances = std::get<AssetDataMap<MaterialInstance>>(m_loadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<MaterialInstance>())
    {
      if(static_cast<uint64_t>(handle.id) < 1000) continue;
      materialInstances.try_emplace(handle, Serializer::DeserializeMaterialInstance(path));
    }

    auto& audioClips = std::get<AssetDataMap<AudioClip>>(m_loadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<AudioClip>())
    {
      if(static_cast<uint64_t>(handle.id) < 1000) continue;
      audioClips.try_emplace(handle, AudioClip::Create(path));
    }
  }

  void AssetManager::SaveDirtyAssets()
  {
    auto saveSet = []<typename T>(AssetSet<T>&dirtySet)
    {
      for (Handle handle : dirtySet)
      {
        std::filesystem::path path = AssetRegistry::GetAssetPath(handle);

        if constexpr (std::is_same_v<T, Handle<Scene>>)
        {
          Serializer::SerializeScene(handle, path);
        }
        else if constexpr (std::is_same_v<T, Handle<Material>>)
        {
          Serializer::SerializeMaterial(handle, path);
        }

        LOG_TRACE("Saved asset to {}", path);
      }

      dirtySet.clear();
    };

    std::apply([&](auto&... sets) { (saveSet(sets), ...); }, m_dirtyAssets);
  }
}