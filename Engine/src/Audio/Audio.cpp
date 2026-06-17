#include "enginepch.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Asset/AssetManager.h"
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace Engine
{
  static ma_engine s_AudioEngine;

  struct Voice
  {
    ma_sound sound{};
    ma_audio_buffer buffer{};
    AudioInstance instance{};
    bool active = false;
    bool paused = false;
  };

  static constexpr uint32_t MaxVoices = 32;
  static Voice s_Voices[MaxVoices];

  static Voice* AcquireVoice(AudioInstance newInstanceId)
  {
    for (uint32_t i = 0; i < MaxVoices; i++)
    {
      if (!s_Voices[i].active)
      {
        s_Voices[i].instance = newInstanceId;
        s_Voices[i].active = true;
        return &s_Voices[i];
      }
    }
    return nullptr;
  }

  static Voice* GetVoice(AudioInstance instance)
  {
    if (!instance) return nullptr;

    for (uint32_t i = 0; i < MaxVoices; i++)
    {
      if (s_Voices[i].active && s_Voices[i].instance == instance)
      {
        return &s_Voices[i];
      }
    }
    return nullptr;
  }

  void Audio::Initialize()
  {
    if (ma_engine_init(nullptr, &s_AudioEngine) != MA_SUCCESS)
    {
      throw std::runtime_error("Failed to initialize audio engine");
    }

    ma_engine_listener_set_position(&s_AudioEngine, 0, 0, 0, 0);
    ma_engine_listener_set_direction(&s_AudioEngine, 0, 0, 0, -1);
    ma_engine_listener_set_world_up(&s_AudioEngine, 0, 0, 1, 0);
  }

  void Audio::Shutdown()
  {
    ma_engine_uninit(&s_AudioEngine);
  }

  void Audio::Update()
  {
    for (uint32_t i = 0; i < MaxVoices; i++)
    {
      Voice& voice = s_Voices[i];

      if (!voice.active)
        continue;

      if (!ma_sound_is_playing(&voice.sound))
      {
        ma_sound_uninit(&voice.sound);
        ma_audio_buffer_uninit(&voice.buffer);
        voice.active = false;
        voice.instance = 0;
      }
    }
  }

  void Audio::SetListener(float px, float py, float pz, float fx, float fy, float fz, float ux, float uy, float uz)
  {
    ma_engine_listener_set_position(&s_AudioEngine, 0, px, py, pz);
    ma_engine_listener_set_direction(&s_AudioEngine, 0, -fx, -fy, -fz);
    ma_engine_listener_set_world_up(&s_AudioEngine, 0, ux, uy, uz);
  }

  void Audio::SetSourcePosition(AudioInstance instance, const glm::vec3& position)
  {
    Voice* voice = GetVoice(instance);
    if (!voice) return;

    ma_sound_set_position(&voice->sound, position.x, position.y, position.z);
  }

  AudioInstance Audio::Play2D(Uuid clip)
  {
    if (!clip)
    {
      LOG_WARNING("AudioClip is invalid/null");
      return Uuid{};
    }

    AudioInstance newInstance = Uuid::Generate();

    Voice* voice = AcquireVoice(newInstance);
    if (!voice)
    {
      LOG_WARNING("No available audio voices");
      return Uuid{};
    }

    AudioClip& audioClip = Engine::AssetManager::GetAsset<AudioClip>(clip);

    ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(
      ma_format_f32,
      audioClip.GetChannels(),
      audioClip.GetTotalFrames(),
      audioClip.GetPCMData(),
      nullptr
    );
    bufferConfig.sampleRate = audioClip.GetSampleRate();

    if (ma_audio_buffer_init(&bufferConfig, &voice->buffer) != MA_SUCCESS)
    {
      LOG_ERROR("Failed to init audio buffer");
      return Uuid{};
    }


    if (ma_sound_init_from_data_source(&s_AudioEngine, &voice->buffer, 0, nullptr, &voice->sound) != MA_SUCCESS)
    {
      LOG_ERROR("Failed to initialize audio voice");
      ma_audio_buffer_uninit(&voice->buffer); // Cleanup
      return Uuid{};
    }

    ma_sound_start(&voice->sound);
    return newInstance;
  }

  AudioInstance Audio::Play3D(Uuid clip, const glm::vec3& position)
  {
    if (!clip)
    {
      LOG_WARNING("AudioClip is invalid/null");
			return Uuid{};
    }

		AudioInstance newInstance = Uuid::Generate();

    Voice* voice = AcquireVoice(newInstance);
    if (!voice)
    {
      LOG_WARNING("No available audio voices");
      return Uuid{};
    }

    Engine::AudioClip& audioClip = Engine::AssetManager::GetAsset<Engine::AudioClip>(clip);

    ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(
      ma_format_f32,
      audioClip.GetChannels(),
      audioClip.GetTotalFrames(),
      audioClip.GetPCMData(),
      nullptr
    );
    bufferConfig.sampleRate = audioClip.GetSampleRate();

    if (ma_audio_buffer_init(&bufferConfig, &voice->buffer) != MA_SUCCESS)
    {
      LOG_ERROR("Failed to init audio buffer");
      return Uuid{};
    }

    // 2. Initialize the sound using the buffer as its source!
    if (ma_sound_init_from_data_source(&s_AudioEngine, &voice->buffer, 0, nullptr, &voice->sound) != MA_SUCCESS)
    {
      LOG_ERROR("Failed to initialize audio voice");
      ma_audio_buffer_uninit(&voice->buffer); // Cleanup
      return Uuid{};
    }

    // 3. Start playing
    ma_sound_set_position(&voice->sound, position.x, position.y, position.z);
    ma_sound_set_spatialization_enabled(&voice->sound, MA_TRUE);
    ma_sound_start(&voice->sound);
    return newInstance;
  }

  void Audio::Stop(AudioInstance instance)
  {
    Voice* voice = GetVoice(instance);
    if (!voice) return;

    if (ma_sound_stop(&voice->sound) != MA_SUCCESS)
    {
      LOG_ERROR("Failed to stop audio");
    }
    else
    {
      ma_sound_uninit(&voice->sound);
      ma_audio_buffer_uninit(&voice->buffer);
      voice->active = false;
      voice->instance = 0;
    }
  }

  bool Audio::IsPlaying(AudioInstance instance)
  {
    Voice* voice = GetVoice(instance);
    if (!voice) return false;

    return ma_sound_is_playing(&voice->sound) == MA_TRUE;
  }

  void Audio::SetVolume(AudioInstance instance, float volume)
  {
    Voice* voice = GetVoice(instance);
    if (!voice) return;

    ma_sound_set_volume(&voice->sound, volume);
  }

  void Audio::SetLooping(AudioInstance instance, bool loop)
  {
    Voice* voice = GetVoice(instance);
    if (!voice) return;

    ma_sound_set_looping(&voice->sound, loop);
  }

  void Audio::StopAll()
  {
    for (uint32_t i = 0; i < MaxVoices; i++)
    {
      Voice& voice = s_Voices[i];

      if (!voice.active) continue;

      ma_sound_stop(&voice.sound);
      ma_sound_uninit(&voice.sound);
      voice.active = false;
      voice.instance = 0;
    }
  }
}