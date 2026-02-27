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
    static void SetListener(float px, float py, float pz, float fx, float fy, float fz, float ux, float uy, float uz);
		static void SetSourcePosition(AudioClip* clip, float x, float y, float z);

    static void Play(AudioClip* clip, bool spatial = true, float x = 0.0f, float y = 0.0f, float z = 0.0f);
		static void Stop(AudioClip* clip);
		static bool IsPlaying(AudioClip* clip);

    static void SetVolume(AudioClip* clip, float volume);
    static void SetLooping(AudioClip* clip, bool loop);

		static void StopAll();

  };
}
