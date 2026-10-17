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
    auto& textures = std::get<AssetDataMap<Texture2D>>(m_LoadedAssets);
    textures.try_emplace(Texture2D::White, TextureSpecification{}, std::array<uint32_t, 1>{0xFFFFFFFF}.data());
    textures.try_emplace(Texture2D::Normal, TextureSpecification{}, std::array<uint32_t, 1>{0xFFFF8080}.data());
    textures.try_emplace(Texture2D::HDR, TextureImporter::LoadTexture2D("resources/hdr/lebombo_4k.hdr"));
    textures.try_emplace(Texture2D::EDITOR_ICONS, TextureImporter::LoadTexture2D("resources/icons/icons.png"));
    textures.try_emplace(Texture2D::DEFAULT_FONT_ATLAS, TextureImporter::LoadTexture2D("resources/fonts/atlas.png"));
    textures.try_emplace(Texture2D::EDITOR_THUMBNAIL_ATLAS, TextureSpecification{ .width = 2048, .height = 2048, .format = engine::GraphicsContext::GetSurfaceFormat() }, nullptr);

    auto& meshes = std::get<AssetDataMap<Mesh>>(m_LoadedAssets);
    meshes.try_emplace(Mesh::Empty, MeshImporter::LoadMesh("resources/models/cube.glb"));
    meshes.try_emplace(Mesh::Cube, MeshImporter::LoadMesh("resources/models/cube.glb"));
    meshes.try_emplace(Mesh::Sphere, MeshImporter::LoadMesh("resources/models/sphere.glb"));

    auto& shaders = std::get<AssetDataMap<Shader>>(m_LoadedAssets);
    shaders.try_emplace(Shader::Pink, ShaderImporter::LoadShader("resources/shaders/pink.wgsl"));
    shaders.try_emplace(Shader::Flat, ShaderImporter::LoadShader("resources/shaders/flat.wgsl"));
    shaders.try_emplace(Shader::UI, ShaderImporter::LoadShader("resources/shaders/ui.wgsl"));
    shaders.try_emplace(Shader::Skybox, ShaderImporter::LoadShader("resources/shaders/skybox.wgsl"));
    shaders.try_emplace(Shader::Normal, ShaderImporter::LoadShader("resources/shaders/normal.wgsl"));

    auto& scenes = std::get<AssetDataMap<Scene>>(m_LoadedAssets);
    scenes.try_emplace(Scene::Default, SceneImporter::LoadScene("resources/scenes/default.scene"));

    auto& materials = std::get<AssetDataMap<Material>>(m_LoadedAssets);
    materials.try_emplace(Material::Pink, MaterialAsset(Shader::Pink));

    auto& materialInstances = std::get<AssetDataMap<MaterialInstance>>(m_LoadedAssets);
    materialInstances.try_emplace(MaterialInstance::Pink, MaterialInstanceData(Material::Pink));

    auto& audioClips = std::get<AssetDataMap<AudioClip>>(m_LoadedAssets);

    for (const auto& [handle, path] : AssetRegistry::GetRecords<Texture2D>())
    {
      textures.try_emplace(handle, TextureImporter::LoadTexture2D(path));
    }

    for (const auto& [handle, path] : AssetRegistry::GetRecords<Mesh>())
    {
      meshes.try_emplace(handle, MeshImporter::LoadMesh(path));
    }

    for (const auto& [handle, path] : AssetRegistry::GetRecords<Shader>())
    {
      shaders.try_emplace(handle, ShaderImporter::LoadShader(path.string()));
    }

    for (const auto& [handle, path] : AssetRegistry::GetRecords<Scene>())
    {
      scenes.try_emplace(handle, SceneImporter::LoadScene(path));
    }

    for (const auto& [handle, path] : AssetRegistry::GetRecords<Material>())
    {
      materials.try_emplace(handle, Serializer::DeserializeMaterial(path));
    }

    for (const auto& [handle, path] : AssetRegistry::GetRecords<MaterialInstance>())
    {
      materialInstances.try_emplace(handle, Serializer::DeserializeMaterialInstance(path));
    }

    for (const auto& [handle, path] : AssetRegistry::GetRecords<AudioClip>())
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
        std::filesystem::path path = AssetRegistry::GetPath(handle);

        if constexpr (std::is_same_v<Handle, Scene>)
        {
          Serializer::SerializeScene(handle, path);
        }
        else if constexpr (std::is_same_v<Handle, Material>)
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