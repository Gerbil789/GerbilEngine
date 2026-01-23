#include "enginepch.h"
#include "IconManager.h"
#include "Engine/Asset/Importer/TextureImporter.h"

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

  static Engine::Texture2D* s_TextureAtlas = nullptr;
  constexpr static glm::ivec2 s_CellSize{ 64, 64 };
  static std::array<Engine::SubTexture2D*, static_cast<size_t>(Icon::Count)> s_Icons;

  void IconManager::Load(const std::filesystem::path& path)
  {
    s_TextureAtlas = Engine::TextureImporter::LoadTexture2D(path);

    const auto addIcon = [](Icon icon, const glm::ivec2& coords)
      {
        s_Icons[static_cast<size_t>(icon)] = Engine::SubTexture2D::CreateFromGrid(s_TextureAtlas, coords, s_CellSize);
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
    delete s_TextureAtlas;
		s_TextureAtlas = nullptr;
  }

  Engine::SubTexture2D* IconManager::GetIcon(Icon icon)
  {
    return s_Icons[static_cast<size_t>(icon)];
  }
}