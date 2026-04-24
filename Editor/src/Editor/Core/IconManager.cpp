#include "IconManager.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Asset/AssetRecord.h"
#include <glm/glm.hpp>
#include <array>

namespace Editor
{
  constexpr std::array<glm::ivec2, static_cast<size_t>(Editor::Icon::Count)> IconCoords
  {
      glm::ivec2{0, 0}, // EmptyFolder
      glm::ivec2{1, 0}, // Folder
      glm::ivec2{2, 0}, // File
      glm::ivec2{3, 0}, // EmptyFile
      glm::ivec2{4, 0}, // Audio
      glm::ivec2{5, 0}, // Landscape
      glm::ivec2{6, 0}, // Image
      glm::ivec2{7, 0}, // Mesh
  };

  static Engine::Texture2D s_TextureAtlas;
  constexpr static glm::ivec2 s_CellSize{ 64, 64 };
  static std::array<Engine::SubTexture2D*, static_cast<size_t>(Icon::Count)> s_Icons;

  void IconManager::Load(const std::filesystem::path& path)
  {
    s_TextureAtlas = Engine::TextureImporter::LoadTexture(path).value();

    const auto addIcon = [](Icon icon, const glm::ivec2& coords)
      {
        s_Icons[static_cast<size_t>(icon)] = Engine::SubTexture2D::CreateFromGrid(&s_TextureAtlas, coords, s_CellSize);
      };

    for (size_t i = 0; i < IconCoords.size(); ++i)
    {
      addIcon(static_cast<Icon>(i), IconCoords[i]);
    }
  }

  void IconManager::Unload()
  {
    for (auto& icon : s_Icons)
    {
      delete icon;
      icon = nullptr;
    }
  }

  Engine::SubTexture2D* IconManager::GetIcon(Icon icon)
  {
    return s_Icons[static_cast<size_t>(icon)];
  }

  Engine::SubTexture2D* IconManager::GetIcon(Engine::AssetType assetType)
  {
    static const std::unordered_map<Engine::AssetType, Icon> map
    {
    {Engine::AssetType::Directory, Icon::Folder},
    {Engine::AssetType::EmptyDirectory, Icon::EmptyFolder},
    {Engine::AssetType::Texture2D, Icon::Image},
    {Engine::AssetType::Scene, Icon::Landscape},
    {Engine::AssetType::Material, Icon::File},
    {Engine::AssetType::Mesh, Icon::Mesh},
    {Engine::AssetType::Audio, Icon::Audio},
    };

    if (auto it = map.find(assetType); it != map.end())
    {
      return GetIcon(it->second);
    }

    return GetIcon(Icon::File);
  }
}