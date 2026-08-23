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

		bool operator==(const SamplerKey&) const = default;
	};
}

namespace std 
{
	template<>
	struct hash<Engine::SamplerKey> 
	{
		inline std::size_t operator()(const Engine::SamplerKey& key) const noexcept 
		{
			return (static_cast<size_t>(key.filter) << 4) ^ static_cast<size_t>(key.wrap);
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

		static wgpu::Sampler GetSampler(SamplerKey key)
		{
			return s_Samplers.at(key);
		}

	private:
		inline static std::unordered_map<SamplerKey, wgpu::Sampler> s_Samplers;

		static wgpu::Sampler CreateSampler(wgpu::Device device, wgpu::FilterMode minMag, wgpu::MipmapFilterMode mip, wgpu::AddressMode mode);

	};
}


