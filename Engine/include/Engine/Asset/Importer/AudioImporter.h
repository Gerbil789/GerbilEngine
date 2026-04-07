#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class AudioClip;

	class ENGINE_API AudioImporter
	{
	public:
		static AudioClip* ImportAudio(const std::filesystem::path& path);
		static AudioClip* LoadAudio(const std::filesystem::path& path);
	};
}