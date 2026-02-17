#include "enginepch.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Log.h"
#include <miniaudio.h>   

namespace Engine
{
	struct AudioClip::Impl
	{
		ma_sound sound;
	};

	AudioClip::AudioClip(const std::filesystem::path& path, void* fence) : m_Impl(std::make_unique<Impl>())
	{
		if(ma_sound_init_from_file(&Audio::GetAudioEngine() , path.string().c_str(), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, nullptr, reinterpret_cast<ma_fence*>(fence), &m_Impl->sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to load audio from file: {}", path);
		}
	}

	AudioClip::~AudioClip()
	{
		ma_sound_uninit(&m_Impl->sound);
	}

	float AudioClip::GetDurationSeconds() const
	{
		ma_uint64 frames = 0;
		ma_sound_get_length_in_pcm_frames(&m_Impl->sound, &frames);
		return static_cast<float>(frames) / ma_engine_get_sample_rate(m_Impl->sound.engineNode.pEngine);
	}

	float AudioClip::GetCurrentTimeSeconds() const
	{
		ma_uint64 cursor = 0;
		ma_sound_get_cursor_in_pcm_frames(&m_Impl->sound, &cursor);

		return static_cast<float>(cursor) / ma_engine_get_sample_rate(m_Impl->sound.engineNode.pEngine);
	}

	void AudioClip::SetCurrentTimeSeconds(float time)
	{
		const ma_uint32 sr = ma_engine_get_sample_rate(m_Impl->sound.engineNode.pEngine);

		ma_sound_seek_to_pcm_frame(&m_Impl->sound, static_cast<ma_uint64>(time * sr));
	}

	ma_sound& AudioClip::GetSound()
	{
		return m_Impl->sound;
	}
}