#include "enginepch.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Engine
{
	TextureCube::TextureCube(const TextureCubeSpecification& specification)
	{
		m_Size = specification.size;
		m_TextureFormat = specification.format;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.label = { "CubemapTexture", WGPU_STRLEN };
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = m_TextureFormat;
		textureDesc.sampleCount = 1;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		textureDesc.size = { m_Size, m_Size, 6 };
		textureDesc.mipLevelCount = specification.mipCount;
		textureDesc.usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::TextureBinding;
		m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = { "CubemapTextureView", WGPU_STRLEN };
		viewDesc.format = m_TextureFormat;
		viewDesc.aspect = wgpu::TextureAspect::All;
		viewDesc.dimension = wgpu::TextureViewDimension::Cube;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = specification.mipCount;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 6;
		m_TextureView = m_Texture.createView(viewDesc);
	}
}