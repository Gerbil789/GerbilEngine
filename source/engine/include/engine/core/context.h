#pragma once

#include "engine/asset/handle.h"
#include "engine/asset/asset_registry.h"
#include "engine/core/scene.h"
#include "engine/core/log.h"

namespace engine
{
  struct Context
  {
    inline static std::filesystem::path projectDirectory;
    inline static std::filesystem::path assetsDirectory;

    inline static SceneHandle activeSceneHandle;

    static void SetActiveScene(SceneHandle handle)
    {
      pendingSceneHandle = handle;
    }

    static void Update()
    {
      activeSceneHandle.Get().DeleteMarkedEntities();

      if(activeSceneHandle != pendingSceneHandle)
      {
        activeSceneHandle = pendingSceneHandle;
        LOG_INFO("Active scene set to '{}' ({})", assetRegistry::GetassetPath(activeSceneHandle).filename(), activeSceneHandle.id);
      }
    }

  private:
    inline static SceneHandle pendingSceneHandle;
  };
}