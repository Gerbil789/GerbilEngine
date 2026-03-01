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
		bool active = false;
	};

	static constexpr uint32_t MaxVoices = 32;
	static Voice s_Voices[MaxVoices];

	static Voice* AcquireVoice()
	{
		for (uint32_t i = 0; i < MaxVoices; i++)
		{
			if (!s_Voices[i].active)
			{
				return &s_Voices[i];
			}
		}
		return nullptr; // no free voices
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

		auto clips = AssetManager::GetAssetsOfType<AudioClip>(AssetType::Audio);

		ma_fence fence;
		if(ma_fence_init(&fence) != MA_SUCCESS)
		{
			throw std::runtime_error("Failed to initialize fence");
		}

		auto records = AssetManager::GetAllAssetRecordsOfType(AssetType::Audio);

		for(auto record : records)
		{
			auto clip = AssetManager::LoadAsset<AudioClip>(*record, record->path, (void*)&fence);
			if(!clip)
			{
				LOG_ERROR("Failed to load audio clip: {}", record->path);
			}
		}

		ma_fence_wait(&fence); // Wait for all sounds to finish loading
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
				voice.active = false;
			}
		}
	}

	ma_engine& Audio::GetAudioEngine()
	{
		return s_AudioEngine;
	}

	void Audio::SetListener(float px, float py, float pz, float fx, float fy, float fz, float ux, float uy, float uz)
	{
		ma_engine_listener_set_position(&s_AudioEngine, 0, px, py, pz);
		ma_engine_listener_set_direction(&s_AudioEngine, 0, -fx, -fy, -fz);
		ma_engine_listener_set_world_up(&s_AudioEngine, 0, ux, uy, uz);
	}

	void Audio::SetSourcePosition(AudioClip* clip, float x, float y, float z)
	{
		if (!clip) return;
		ma_sound_set_position(&clip->GetSound(), x, y, z);
	}

	void Audio::Play2D(AudioClip* clip)
	{
		if (!clip)
		{
			LOG_WARNING("AudioClip is null");
			return;
		}

		Voice* voice = AcquireVoice();
		if (!voice)
		{
			LOG_WARNING("No available audio voices");
			return;
		}

		if (ma_sound_init_copy(&s_AudioEngine, &clip->GetSound(), 0, nullptr, &voice->sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to initialize audio voice");
			return;
		}

		if (ma_sound_start(&voice->sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to start audio voice");
			return;
		}

		voice->active = true;
	}

	void Audio::Play3D(AudioClip* clip, float x, float y, float z)
	{
		if(!clip)
		{
			LOG_WARNING("AudioClip is null");
			return;
		}

		Voice* voice = AcquireVoice();
		if (!voice)
		{
			LOG_WARNING("No available audio voices");
			return;
		}

		if (ma_sound_init_copy(&s_AudioEngine, &clip->GetSound(), 0, nullptr, &voice->sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to initialize audio voice");
			return;
		}

		ma_sound_set_position(&voice->sound, x, y, z);
		ma_sound_set_spatialization_enabled(&voice->sound, MA_TRUE);

		if (ma_sound_start(&voice->sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to start audio voice");
			return;
		}

		voice->active = true;
	}

	void Audio::Stop(AudioClip* clip)
	{
		if (!clip)
		{
			LOG_WARNING("AudioClip is null");
			return;
		}

		ma_sound& sound = clip->GetSound();

		if (ma_sound_stop(&sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to stop audio");
		}
	}

	bool Audio::IsPlaying(AudioClip* clip)
	{
		if (!clip)
		{
			LOG_ERROR("AudioClip is null");
			return false;
		}

		return ma_sound_is_playing(&clip->GetSound()) == MA_TRUE;
	}

	void Audio::SetVolume(AudioClip* clip, float volume)
	{
		if (!clip)
		{
			LOG_ERROR("AudioClip is null");
			return;
		}

		ma_sound_set_volume(&clip->GetSound(), volume);
	}

	void Audio::SetLooping(AudioClip* clip, bool loop)
	{
		if (!clip)
		{
			LOG_ERROR("AudioClip is null");
			return;
		}

		ma_sound_set_looping(&clip->GetSound(), loop);
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
		}
	}
}