#pragma once

#include <filesystem>

namespace Engine
{
  struct EngineContext
  {
		std::filesystem::path ProjectDirectory;
    std::filesystem::path AssetsDirectory;
  };
}
