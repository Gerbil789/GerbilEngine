#pragma once

#include "Engine/Renderer/Texture.h"
#include <unordered_map>
#include <string>

namespace Editor
{
  class EditorIcons
  {
  public:
    static void Init(const Ref<Engine::Texture2D>& iconSheet, const glm::ivec2& cellSize)
    {
      s_IconSheet = iconSheet;
      s_CellSize = cellSize;

      AddIcon("EmptyFolder", { 0, 0 });
      AddIcon("Folder", { 1, 0 });
      AddIcon("File", { 2, 0 });
      AddIcon("Image", { 3, 0 });
      AddIcon("Scene", { 4, 0 });
    }

		//TODO: return default icon if not found
		//TODO: use enums for faster lookup
    static Ref<Engine::SubTexture2D> GetIcon(const std::string& name)
    {
      auto it = s_Icons.find(name);
      if (it != s_Icons.end())
      {
        return it->second;
      }

      return nullptr;
    }

  private:
    static void AddIcon(const std::string& name, const glm::ivec2& coords)
    {
      s_Icons[name] = Engine::SubTexture2D::CreateFromGrid(s_IconSheet, coords, s_CellSize);
    }

  private:
    inline static Ref<Engine::Texture2D> s_IconSheet;
    inline static glm::ivec2 s_CellSize;
    inline static std::unordered_map<std::string, Ref<Engine::SubTexture2D>> s_Icons;
  };
}
