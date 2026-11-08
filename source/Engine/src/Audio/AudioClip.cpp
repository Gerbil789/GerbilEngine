#include "enginepch.h"
#include "Engine/Audio/AudioClip.h"
#include <miniaudio.h>   

namespace engine
{
	AudioClip AudioClip::Create(const std::filesystem::path& path)
	{
		ma_decoder decoder;
		ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);

		if (ma_decoder_init_file(path.string().c_str(), &config, &decoder) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to load audio file: {}", path);
			return{};
		}

		AudioClip clip;

		clip.channels = decoder.outputChannels;
		clip.sampleRate = decoder.outputSampleRate;

		ma_uint64 total;
		ma_decoder_get_length_in_pcm_frames(&decoder, &total);
		clip.totalFrames = static_cast<uint64_t>(total);

		clip.PCMData.resize(clip.totalFrames * clip.channels);
		ma_decoder_read_pcm_frames(&decoder, clip.PCMData.data(), clip.totalFrames, nullptr);

		ma_decoder_uninit(&decoder);

		return clip;
	}
}