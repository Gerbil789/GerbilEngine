#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "AudioClip.h"

namespace Engine
{
  class Audio
  {
  public:
    static void Initialize();
    static void Shutdown();

    static void Play(Ref<AudioClip> clip);

    static void StopAllSounds();
  };
}
