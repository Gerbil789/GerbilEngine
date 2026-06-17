#pragma once

#include "Engine/Core/UUID.h"
#include <glm/glm.hpp>

class ma_engine;

namespace Engine
{
  using AudioInstance = Uuid;

  class ENGINE_API Audio
  {
  public:
    static void Initialize();
    static void Shutdown();

    static void Update();

    static void SetListener(float px, float py, float pz, float fx, float fy, float fz, float ux, float uy, float uz);

    static AudioInstance Play2D(Uuid clip);
    static AudioInstance Play3D(Uuid clip, const glm::vec3& position);

    static void SetSourcePosition(AudioInstance instance, const glm::vec3& position);
		static void Stop(AudioInstance instance);
		static bool IsPlaying(AudioInstance instance);

    static void SetVolume(AudioInstance instance, float volume);
    static void SetLooping(AudioInstance instance, bool loop);

		static void StopAll();
  };
}
