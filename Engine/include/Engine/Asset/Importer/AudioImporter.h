#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	class AudioClip;

	class ENGINE_API AudioImporter
	{
	public:
		static std::optional<AudioClip> LoadAudioClip(const std::filesystem::path& path);
	};
}