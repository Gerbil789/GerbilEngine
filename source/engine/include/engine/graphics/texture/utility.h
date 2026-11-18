#pragma once

#include "engine/asset/handle.h"
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	class textureCube;

	uint32_t GetMaxMipLevelCount(uint32_t width, uint32_t height);
	textureCube EquirectangularToCubemap(TextureHandle equirectangulartexture);

	void GenerateMipmaps(wgpu::Texture texture);
}