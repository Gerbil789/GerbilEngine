#include "enginepch.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace engine
{
	TextureCube::TextureCube(const TextureCubeSpecification& specification)
	{
		m_size = specification.size;
		m_textureFormat = specification.format;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.label = "CubemapTexture";
		textureDesc.dimension = wgpu::TextureDimension::e2D;
		textureDesc.format = m_textureFormat;
		textureDesc.sampleCount = 1;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		textureDesc.size = { m_size, m_size, 6 };
		textureDesc.mipLevelCount = specification.mipCount;
		textureDesc.usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::TextureBinding;
		m_texture = GraphicsContext::GetDevice().CreateTexture(&textureDesc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = "CubemapTextureView";
		viewDesc.format = m_textureFormat;
		viewDesc.aspect = wgpu::TextureAspect::All;
		viewDesc.dimension = wgpu::TextureViewDimension::Cube;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = specification.mipCount;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 6;
		m_textureView = m_texture.CreateView(&viewDesc);
	}
}