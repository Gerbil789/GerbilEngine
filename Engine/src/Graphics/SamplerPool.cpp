#include "enginepch.h"
#include "Engine/Graphics/SamplerPool.h"

namespace Engine
{
	void SamplerPool::Initialize()
	{
		auto device = GraphicsContext::GetDevice();
		s_Samplers[{ TextureFilter::Point, TextureWrap::Repeat }] = CreateSampler(device, wgpu::FilterMode::Nearest, wgpu::MipmapFilterMode::Nearest, wgpu::AddressMode::Repeat);
		s_Samplers[{ TextureFilter::Point, TextureWrap::Clamp }] = CreateSampler(device, wgpu::FilterMode::Nearest, wgpu::MipmapFilterMode::Nearest, wgpu::AddressMode::ClampToEdge);
		s_Samplers[{ TextureFilter::Point, TextureWrap::Mirror }] = CreateSampler(device, wgpu::FilterMode::Nearest, wgpu::MipmapFilterMode::Nearest, wgpu::AddressMode::MirrorRepeat);
		s_Samplers[{ TextureFilter::Bilinear, TextureWrap::Repeat }] = CreateSampler(device, wgpu::FilterMode::Linear, wgpu::MipmapFilterMode::Nearest, wgpu::AddressMode::Repeat);
		s_Samplers[{ TextureFilter::Bilinear, TextureWrap::Clamp }] = CreateSampler(device, wgpu::FilterMode::Linear, wgpu::MipmapFilterMode::Nearest, wgpu::AddressMode::ClampToEdge);
		s_Samplers[{ TextureFilter::Bilinear, TextureWrap::Mirror }] = CreateSampler(device, wgpu::FilterMode::Linear, wgpu::MipmapFilterMode::Nearest, wgpu::AddressMode::MirrorRepeat);
		s_Samplers[{ TextureFilter::Trilinear, TextureWrap::Repeat }] = CreateSampler(device, wgpu::FilterMode::Linear, wgpu::MipmapFilterMode::Linear, wgpu::AddressMode::Repeat);
		s_Samplers[{ TextureFilter::Trilinear, TextureWrap::Clamp }] = CreateSampler(device, wgpu::FilterMode::Linear, wgpu::MipmapFilterMode::Linear, wgpu::AddressMode::ClampToEdge);
		s_Samplers[{ TextureFilter::Trilinear, TextureWrap::Mirror }] = CreateSampler(device, wgpu::FilterMode::Linear, wgpu::MipmapFilterMode::Linear, wgpu::AddressMode::MirrorRepeat);
	}

	void SamplerPool::Shutdown()
	{
		s_Samplers.clear();
	}

	wgpu::Sampler SamplerPool::CreateSampler(wgpu::Device device, wgpu::FilterMode minMag, wgpu::MipmapFilterMode mip, wgpu::AddressMode mode)
	{
		wgpu::SamplerDescriptor desc{};
		desc.minFilter = minMag;
		desc.magFilter = minMag;
		desc.mipmapFilter = mip;
		desc.addressModeU = mode;
		desc.addressModeV = mode;
		desc.addressModeW = mode;
		desc.maxAnisotropy = 1;
		return device.createSampler(desc);
	}
}