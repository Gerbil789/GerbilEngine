#pragma once

#include "Engine/Core/API.h"
#include "AudioClip.h"

namespace Engine
{
  class ENGINE_API Audio
  {
  public:
    static void Initialize();
    static void Shutdown();

    static void Play(AudioClip* clip);

    static void StopAllSounds();
  };
}
