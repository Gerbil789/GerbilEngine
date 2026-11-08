#pragma once

#include "Engine/Asset/Handle.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Log.h"

namespace engine
{
  struct Context
  {
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
        LOG_INFO("Active scene set to '{}' ({})", AssetRegistry::GetAssetPath(activeSceneHandle).filename(), activeSceneHandle.id);
      }
    }

  private:
    inline static SceneHandle pendingSceneHandle;
  };
}