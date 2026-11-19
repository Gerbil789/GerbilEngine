#pragma once

#include "engine/asset/handle.h"
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	struct TextureCube;

	uint32_t GetMaxMipLevelCount(uint32_t width, uint32_t height);
	TextureCube EquirectangularToCubemap(TextureHandle equirectangularTextureHandle);

	void GenerateMipmaps(wgpu::Texture texture);
}