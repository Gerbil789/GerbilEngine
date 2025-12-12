#pragma once

#include "Engine/Renderer/Texture.h"

namespace Editor
{
  enum class Icon
  {
    EmptyFolder,
    Folder,
    File,
    EmptyFile,
    Audio,
    Landscape,
    Image,
    Mesh,
  };


  class EditorIcons
  {
  public:
    static void Load(const Ref<Engine::Texture2D> textureAtlas)
    {
      s_TextureAtlas = textureAtlas;

      AddIcon(Icon::EmptyFolder, { 0, 0 });
      AddIcon(Icon::Folder, { 1, 0 });
      AddIcon(Icon::File, { 2, 0 });
      AddIcon(Icon::EmptyFile, { 3, 0 });
      AddIcon(Icon::Audio, { 4, 0 });
      AddIcon(Icon::Landscape, { 5, 0 });
      AddIcon(Icon::Image, { 6, 0 });
      AddIcon(Icon::Mesh, { 7, 0 });
    }

    static Ref<Engine::SubTexture2D> GetIcon(Icon icon)
    {
      return s_Icons[(size_t)icon];
    }

  private:
    static void AddIcon(Icon icon, const glm::ivec2& coords)
    {
      s_Icons[(size_t)icon] = Engine::SubTexture2D::CreateFromGrid(s_TextureAtlas, coords, s_CellSize);
    }

  private:
    inline static Ref<Engine::Texture2D> s_TextureAtlas;
		inline static glm::ivec2 s_CellSize{ 64, 64 };
    inline static std::array<Ref<Engine::SubTexture2D>, 8> s_Icons;
  };
}
