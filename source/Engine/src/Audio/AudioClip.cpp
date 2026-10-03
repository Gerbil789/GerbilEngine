#include "enginepch.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Log.h"
#include <miniaudio.h>   

namespace engine
{
	AudioClipAsset::AudioClipAsset(const std::filesystem::path& path)
	{
		ma_decoder decoder;
		// 0, 0 means "use the file's native channel count and sample rate"
		ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);

		if (ma_decoder_init_file(path.string().c_str(), &config, &decoder) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to load audio file: {0}", path.string());
			return;
		}

		channels = decoder.outputChannels;
		sampleRate = decoder.outputSampleRate;

		ma_uint64 total;
		ma_decoder_get_length_in_pcm_frames(&decoder, &total);
		totalFrames = static_cast<uint64_t>(total);

		PCMData.resize(totalFrames * channels);
		ma_decoder_read_pcm_frames(&decoder, PCMData.data(), totalFrames, nullptr);

		ma_decoder_uninit(&decoder);
	}


	AudioClipAsset& AudioClip::Get() const
	{
		return AssetManager::GetAsset(*this);
	}

	//TODO: use template to specify units?
	float AudioClip::GetDurationSeconds() const
	{
		AudioClipAsset& data = Get();
		if (data.sampleRate == 0) return 0.0f;
		return static_cast<float>(data.totalFrames) / static_cast<float>(data.sampleRate);
	}

	const float* AudioClip::GetPCMData() const
	{
		return Get().PCMData.data();
	}

	uint64_t AudioClip::GetTotalFrames() const
	{
		return Get().totalFrames;
	}

	uint32_t AudioClip::GetChannels() const
	{
		return Get().channels;
	}

	uint32_t AudioClip::GetSampleRate() const
	{
		return Get().sampleRate;
	}
}