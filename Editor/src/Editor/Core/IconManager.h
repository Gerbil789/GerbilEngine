#pragma once

#include <filesystem>

namespace Engine
{
  class SubTexture2D;
  enum class AssetType;
}

namespace Editor
{
	enum class Icon //TODO: add invalid icon for debugging
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
    static Engine::SubTexture2D* GetIcon(Engine::AssetType assetType);
  };
}
