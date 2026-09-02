#pragma once

#include "Engine/Asset/AssetHandle.h"
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	class TextureCube;

	uint32_t GetMaxMipLevelCount(const wgpu::Extent3D& textureSize);
	TextureCube EquirectangularToCubemap(Texture2D equirectangularTexture);

	void GenerateMipmaps(wgpu::Texture texture);
}