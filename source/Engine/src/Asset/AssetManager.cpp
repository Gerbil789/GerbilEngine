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
    auto addBuiltIn = [&]<typename T, typename... Args>(Handle<T> handle, const std::string& nameOrPath, Args&&... args)
    {
      AssetRegistry::AddRecord(handle, nameOrPath);
      
      auto& assetMap = std::get<AssetDataMap<T>>(m_LoadedAssets);
      assetMap.try_emplace(handle, std::forward<Args>(args)...);
    };

    addBuiltIn(texture::White, "White Texture", TextureSpecification{}, std::array<uint32_t, 1>{0xFFFFFFFF}.data());
    addBuiltIn(texture::Normal, "Normal Texture", TextureSpecification{}, std::array<uint32_t, 1>{0xFFFF8080}.data());
    addBuiltIn(texture::HDR, "resources/hdr/autumn_field_puresky_4k.hdr", TextureImporter::LoadTexture2D("resources/hdr/autumn_field_puresky_4k.hdr"));
    addBuiltIn(texture::EditorIcons, "resources/icons/icons.png", TextureImporter::LoadTexture2D("resources/icons/icons.png"));
    addBuiltIn(texture::DefaultFontAtlas, "resources/fonts/atlas.png", TextureImporter::LoadTexture2D("resources/fonts/atlas.png"));
    addBuiltIn(texture::EditorThumbnailAtlas, "Editor Thumbnail Atlas", TextureSpecification{ .width = 2048, .height = 2048, .format = engine::GraphicsContext::GetSurfaceFormat() }, nullptr);

    addBuiltIn(mesh::Empty, "Empty Mesh", MeshImporter::LoadMesh("resources/models/cube.glb"));
    addBuiltIn(mesh::Cube, "resources/models/cube.glb", MeshImporter::LoadMesh("resources/models/cube.glb"));
    addBuiltIn(mesh::Sphere, "resources/models/sphere.glb", MeshImporter::LoadMesh("resources/models/sphere.glb"));

    addBuiltIn(shader::Pink, "resources/shaders/pink.wgsl", ShaderImporter::LoadShader("resources/shaders/pink.wgsl"));
    addBuiltIn(shader::Flat, "resources/shaders/flat.wgsl", ShaderImporter::LoadShader("resources/shaders/flat.wgsl"));
    addBuiltIn(shader::UI, "resources/shaders/ui.wgsl", ShaderImporter::LoadShader("resources/shaders/ui.wgsl"));
    addBuiltIn(shader::Skybox, "resources/shaders/skybox.wgsl", ShaderImporter::LoadShader("resources/shaders/skybox.wgsl"));
    addBuiltIn(shader::Normal, "resources/shaders/normal.wgsl", ShaderImporter::LoadShader("resources/shaders/normal.wgsl"));

    addBuiltIn(scene::Empty, "Empty Scene", Scene());
    addBuiltIn(scene::Default, "resources/scenes/default.scene", SceneImporter::LoadScene("resources/scenes/default.scene"));
    
    addBuiltIn(material::Pink, "Pink Material", Material(shader::Pink));
    addBuiltIn(material::Pink, "White Material", Material(shader::Flat));
    
    addBuiltIn(material_instance::Pink, "Pink Material Instance", MaterialInstance(material::Pink));
    addBuiltIn(material_instance::Pink, "White Material Instance", MaterialInstance(material::White)); //TODO: white material is not working, it fallbacks to the pink

    addBuiltIn(audio::Empty, "Empty Audio Clip", AudioClip{});

    auto& textures = std::get<AssetDataMap<Texture2D>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Texture2D>())
    {
      textures.try_emplace(handle, TextureImporter::LoadTexture2D(path));
    }

    auto& meshes = std::get<AssetDataMap<Mesh>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Mesh>())
    {
      meshes.try_emplace(handle, MeshImporter::LoadMesh(path));
    }

    auto& shaders = std::get<AssetDataMap<Shader>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Shader>())
    {
      shaders.try_emplace(handle, ShaderImporter::LoadShader(path.string()));
    }

    auto& scenes = std::get<AssetDataMap<Scene>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Scene>())
    {
      scenes.try_emplace(handle, SceneImporter::LoadScene(path));
    }

    auto& materials = std::get<AssetDataMap<Material>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<Material>())
    {
      materials.try_emplace(handle, Serializer::DeserializeMaterial(path));
    }

    auto& materialInstances = std::get<AssetDataMap<MaterialInstance>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<MaterialInstance>())
    {
      materialInstances.try_emplace(handle, Serializer::DeserializeMaterialInstance(path));
    }

    auto& audioClips = std::get<AssetDataMap<AudioClip>>(m_LoadedAssets);
    for (const auto& [handle, path] : AssetRegistry::GetRecords<AudioClip>())
    {
      audioClips.try_emplace(handle, AudioImporter::LoadAudioClip(path));
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

    std::apply([&](auto&... sets) { (saveSet(sets), ...); }, m_DirtyAssets);
  }
}