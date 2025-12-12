#pragma once

#include "Engine/Core/Log.h"
#include "AudioClip.h"

namespace Engine
{
  class Audio
  {
  public:
    static void Initialize();
    static void Shutdown();

    static void StopAllSounds();

    static ma_engine& GetEngine();
    static ma_sound* Play(Ref<AudioClip> clip);
		static void Stop(ma_sound* sound);
  };
}
