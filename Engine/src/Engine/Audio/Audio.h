#pragma once

#include "AudioClip.h"

namespace Engine
{
  class Audio
  {
  public:
    static void Initialize();
    static void Shutdown();

    static void Play(AudioClip* clip);

    static void StopAllSounds();
  };
}
