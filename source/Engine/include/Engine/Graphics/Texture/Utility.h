#pragma once

#include "Engine/Asset/Handle.h"
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	class TextureCube;

	uint32_t GetMaxMipLevelCount(uint32_t width, uint32_t height);
	TextureCube EquirectangularToCubemap(TextureHandle equirectangularTexture);

	void GenerateMipmaps(wgpu::Texture texture);
}