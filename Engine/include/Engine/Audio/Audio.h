#pragma once

#include "Engine/Core/API.h"

class ma_engine;

namespace Engine
{
	class AudioClip;

  class ENGINE_API Audio
  {
  public:
    static void Initialize();
    static void Shutdown();

    static ma_engine& GetAudioEngine();

    static void Play(AudioClip* clip);
		static void Stop(AudioClip* clip);
		static bool IsPlaying(AudioClip* clip);

    static void SetVolume(AudioClip* clip, float volume);
    static void SetLooping(AudioClip* clip, bool loop);

  };
}
