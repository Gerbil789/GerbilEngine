#pragma once

#include "Engine/Asset/Asset.h"
#include <filesystem>

class ma_sound;

namespace Engine
{
	class ENGINE_API AudioClip : public Asset
	{
	public:
		AudioClip();
		AudioClip(const std::filesystem::path& path);
		~AudioClip();

		// 1. Declare Move Constructor and Move Assignment
		AudioClip(AudioClip&& other) noexcept;
		AudioClip& operator=(AudioClip&& other) noexcept;

		// 2. Explicitly delete Copying (optional, but good practice for GPU resources)
		AudioClip(const AudioClip&) = delete;
		AudioClip& operator=(const AudioClip&) = delete;

		float GetDurationSeconds() const;
		float GetCurrentTimeSeconds() const;
		void  SetCurrentTimeSeconds(float time);

		ma_sound& GetSound();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_Impl;
	};
}