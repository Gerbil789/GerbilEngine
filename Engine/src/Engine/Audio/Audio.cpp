#include "enginepch.h"
#include "Audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace Engine
{
	static ma_engine s_AudioEngine;
	static std::vector<ma_sound*> s_ActiveSounds;

	void Audio::Initialize()
	{
		if (ma_engine_init(NULL, &s_AudioEngine) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to initialize audio engine.");
			return;
		}
	}

	void Audio::Shutdown()
	{
		StopAllSounds();
		ma_engine_uninit(&s_AudioEngine);
	}


	void Audio::Play(AudioClip* clip)
	{
		if(!clip)
		{
			LOG_ERROR("AudioClip is null.");
			return;
		}

		ma_audio_buffer* buffer = new ma_audio_buffer();
		ma_audio_buffer_config config = ma_audio_buffer_config_init(
			ma_format_f32,
			clip->GetChannels(),
			clip->GetTotalFrames(),
			clip->GetPCMFrames().data(),
			NULL
		);

		if (ma_audio_buffer_init(&config, buffer) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to init audio buffer");
			delete buffer;
			return;
		}

		ma_sound* sound = new ma_sound();
		if (ma_sound_init_from_data_source(&s_AudioEngine, buffer, 0, NULL, sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to init sound from buffer");
			ma_audio_buffer_uninit(buffer);
			delete buffer;
			delete sound;
			return;
		}

		// Store buffer pointer in the sound so we can delete it later.
		sound->pDataSource = buffer;

		s_ActiveSounds.push_back(sound);
		ma_sound_start(sound);
	}

	void Audio::StopAllSounds()
	{
		for (auto* sound : s_ActiveSounds)
		{
			ma_sound_stop(sound);
			ma_sound_uninit(sound);
			delete sound;
		}
		s_ActiveSounds.clear();
	}
}