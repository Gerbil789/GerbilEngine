#pragma once

#include "Engine/Graphics/Texture/Texture2D.h"
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	class TextureCube;

	uint32_t GetMaxMipLevelCount(uint32_t width, uint32_t height);
	TextureCube EquirectangularToCubemap(Texture2D equirectangularTexture);

	void GenerateMipmaps(wgpu::Texture texture);
}