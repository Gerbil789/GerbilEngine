#include "enginepch.h"
#include "Audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace Engine
{
	ma_engine engine;
	static std::vector<ma_sound*> s_ActiveSounds;

	void Audio::Initialize()
	{
		if (ma_engine_init(NULL, &engine) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to initialize audio engine.");
			return;
		}
	}

	void Audio::Shutdown()
	{
		StopAllSounds();
		ma_engine_uninit(&engine);
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

	ma_engine& Audio::GetEngine()
	{
		return engine;
	}

	ma_sound* Audio::Play(Ref<AudioClip> clip)
	{
		if(!clip)
		{
			LOG_ERROR("AudioClip is null.");
			return nullptr;
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
			return nullptr;
		}

		ma_sound* sound = new ma_sound();
		if (ma_sound_init_from_data_source(&engine, buffer, 0, NULL, sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to init sound from buffer");
			ma_audio_buffer_uninit(buffer);
			delete buffer;
			delete sound;
			return nullptr;
		}

		// Store buffer pointer in the sound so we can delete it later.
		sound->pDataSource = buffer;

		s_ActiveSounds.push_back(sound);
		ma_sound_start(sound);
		return sound;
	}

	void Audio::Stop(ma_sound* sound)
	{
		auto it = std::find(s_ActiveSounds.begin(), s_ActiveSounds.end(), sound);
		if (it != s_ActiveSounds.end())
		{
			ma_sound_stop(sound);
			ma_sound_uninit(sound);
			delete sound;
			s_ActiveSounds.erase(it);
		}
	}

}