#pragma once

#include "Engine/Asset/Asset.h"
#include <filesystem>

class ma_sound;

namespace Engine
{
	class ENGINE_API AudioClip : public Asset
	{
	public:
		AudioClip(const std::filesystem::path& path, void* fence = nullptr);
		~AudioClip();

		float GetDurationSeconds() const;
		float GetCurrentTimeSeconds() const;
		void  SetCurrentTimeSeconds(float time);

		ma_sound& GetSound();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_Impl;
	};
}