#pragma once

#include "Engine/Graphics/GraphicsContext.h"
#include <functional>

namespace Engine
{
	enum class TextureFilter { Point, Bilinear, Trilinear };
	enum class TextureWrap { Repeat, Clamp, Mirror };

	struct SamplerKey
	{
		TextureFilter filter;
		TextureWrap wrap;

		bool operator==(const SamplerKey& other) const
		{
			return filter == other.filter && wrap == other.wrap;
		}
	};

}

namespace std {
	template<>
	struct hash<Engine::SamplerKey> {
		inline std::size_t operator()(const Engine::SamplerKey& key) const noexcept {
			return (static_cast<size_t>(key.filter) << 4) ^
				static_cast<size_t>(key.wrap);
		}
	};
}

namespace Engine
{
	class SamplerPool
	{
	public:
		static void Initialize();
		static void Shutdown();

		static wgpu::Sampler GetSampler(TextureFilter filter, TextureWrap wrap)
		{
			SamplerKey key{ filter, wrap };
			return s_Samplers[key];
		}

	private:
		inline static std::unordered_map<SamplerKey, wgpu::Sampler> s_Samplers;

		static wgpu::Sampler CreateSampler(wgpu::Device device, wgpu::FilterMode minMag, wgpu::MipmapFilterMode mip, wgpu::AddressMode mode);

	};
}


