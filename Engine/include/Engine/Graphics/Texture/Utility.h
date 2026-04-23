#pragma once

#include <webgpu/webgpu-raii.hpp>

namespace Engine
{
	uint32_t GetMaxMipLevelCount(const wgpu::Extent3D& textureSize);

	void GenerateMipmaps(wgpu::Texture texture);
	void ImportanceSample(wgpu::Texture texture);
	void ComputeIrradiance(wgpu::Texture sourceCubemap, wgpu::Texture targetCubemap);
}