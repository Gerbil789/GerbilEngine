#pragma once

#include <webgpu/webgpu_cpp.h>

namespace Engine
{
	class TextureCube;

	uint32_t GetMaxMipLevelCount(const wgpu::Extent3D& textureSize);
	TextureCube EquirectangularToCubemap(Uuid equirectangularTexture);

	void GenerateMipmaps(wgpu::Texture texture);
}