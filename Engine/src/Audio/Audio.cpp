#include "enginepch.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Asset/AssetManager.h"
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace Engine
{
	static ma_engine s_AudioEngine;
	static std::vector<AudioClip*> s_AllClips;

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
			LOG_ERROR("Failed to initialize fence");
		}

		auto records = AssetManager::GetAllAssetRecordsOfType(AssetType::Audio);

		for(auto record : records)
		{
			AudioClip* clip = AssetManager::LoadAsset<AudioClip>(*record, record->path, (void*)&fence);
			if (clip)
			{
				s_AllClips.push_back(clip);
			}

		}

		ma_fence_wait(&fence); // Wait for all sounds to finish loading
		LOG_INFO("Loaded {} audio files", records.size());
	}

	void Audio::Shutdown()
	{
		ma_engine_uninit(&s_AudioEngine);
	}

	ma_engine& Audio::GetAudioEngine()
	{
		return s_AudioEngine;
	}

	void Audio::SetListener(float px, float py, float pz, float fx, float fy, float fz, float ux, float uy, float uz)
	{
		ma_engine_listener_set_position(&s_AudioEngine, 0, px, py, pz);
		ma_engine_listener_set_direction(&s_AudioEngine, 0, fx, fy, fz);
		ma_engine_listener_set_world_up(&s_AudioEngine, 0, ux, uy, uz);
	}

	void Audio::SetSourcePosition(AudioClip* clip, float x, float y, float z)
	{
		if (!clip) return;
		ma_sound_set_position(&clip->GetSound(), x, y, z);
	}

	void Audio::Play(AudioClip* clip, bool spatial, float x, float y, float z)
	{
		if(!clip)
		{
			LOG_WARNING("AudioClip is null.");
			return;
		}
		ma_sound& sound = clip->GetSound();

		ma_sound_set_position(&sound, x, y, z);
		ma_sound_set_spatialization_enabled(&sound, spatial ? MA_TRUE : MA_FALSE);

		ma_sound_set_fade_in_pcm_frames(&sound, 0, 1, 128);
		if (ma_sound_start(&sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to start audio.");
		}
	}

	void Audio::Stop(AudioClip* clip)
	{
		if (!clip)
		{
			LOG_WARNING("AudioClip is null.");
			return;
		}

		ma_sound& sound = clip->GetSound();

		if (ma_sound_stop(&sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to stop audio.");
		}
	}

	bool Audio::IsPlaying(AudioClip* clip)
	{
		if (!clip)
		{
			LOG_ERROR("AudioClip is null.");
			return false;
		}

		return ma_sound_is_playing(&clip->GetSound()) == MA_TRUE;
	}

	void Audio::SetVolume(AudioClip* clip, float volume)
	{
		if (!clip)
		{
			LOG_ERROR("AudioClip is null.");
			return;
		}

		ma_sound_set_volume(&clip->GetSound(), volume);
	}

	void Audio::SetLooping(AudioClip* clip, bool loop)
	{
		if (!clip)
		{
			LOG_ERROR("AudioClip is null.");
			return;
		}

		ma_sound_set_looping(&clip->GetSound(), loop);
	}

	void Audio::StopAll()
	{
		for (auto* clip : s_AllClips)
		{
			if (!clip) continue;

			ma_sound& sound = clip->GetSound();

			ma_sound_stop(&sound);
			ma_sound_seek_to_pcm_frame(&sound, 0);
		}
	}
}