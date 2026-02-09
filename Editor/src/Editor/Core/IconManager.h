#pragma once

#include <filesystem>

namespace Engine
{
  class SubTexture2D;
}

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
    Count
  };

  class IconManager
  {
  public:
    static void Load(const std::filesystem::path& path);
		static void Unload();
    static Engine::SubTexture2D* GetIcon(Icon icon);
  };
}
