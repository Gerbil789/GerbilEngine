#pragma once

#include "engine/asset/handle.h"
#include <glm/glm.hpp>

class ma_engine;

namespace engine
{
  using AudioInstance = AudioClipHandle;
}

namespace engine::audio
{
  void Initialize();
  void Shutdown();

  void Update();

  void SetListener(float px, float py, float pz, float fx, float fy, float fz, float ux, float uy, float uz);

  AudioInstance Play2D(AudioClipHandle clip);
  AudioInstance Play3D(AudioClipHandle clip, const glm::vec3& position);

  void SetSourcePosition(AudioInstance instance, const glm::vec3& position);
  void Stop(AudioInstance instance);
  bool IsPlaying(AudioInstance instance);

  void SetVolume(AudioInstance instance, float volume);
  void SetLooping(AudioInstance instance, bool loop);

  void StopAll();
}
