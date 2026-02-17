#include "enginepch.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Asset/AssetManager.h"
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

// https://miniaud.io/docs/manual/index.html#Introduction

namespace Engine
{
	static ma_engine s_AudioEngine;

	void Audio::Initialize()
	{
		if (ma_engine_init(nullptr, &s_AudioEngine) != MA_SUCCESS)
		{
			throw std::runtime_error("Failed to initialize audio engine");
		}


		auto clips = AssetManager::GetAssetsOfType<AudioClip>(AssetType::Audio);

		ma_fence fence;

		if(ma_fence_init(&fence) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to initialize fence");
		}

		auto records = AssetManager::GetAllAssetRecordsOfType(AssetType::Audio);

		for(auto record : records)
		{
			AssetManager::LoadAsset<AudioClip>(*record, record->path, (void*)&fence);
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

	void Audio::Play(AudioClip* clip)
	{
		if(!clip)
		{
			LOG_WARNING("AudioClip is null.");
			return;
		}

		ma_sound& sound = clip->GetSound();

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
}