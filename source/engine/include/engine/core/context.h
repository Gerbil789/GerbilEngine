#pragma once

#include "engine/asset/handle.h"

namespace engine::context
{
  inline std::filesystem::path projectDirectory;
  inline std::filesystem::path assetsDirectory;

  inline SceneHandle activeSceneHandle;
}