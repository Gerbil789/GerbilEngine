#include "enginepch.h"
#include "engine/graphics/texture/texture_cube.h"
#include "engine/graphics/graphics_context.h"

namespace engine
{
	textureCube::textureCube(const textureCubeSpecification& specification)
	{
		m_size = specification.size;
		m_textureFormat = specification.format;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.label = "Cubemaptexture";
		textureDesc.dimension = wgpu::TextureDimension::e2D;
		textureDesc.format = m_textureFormat;
		textureDesc.sampleCount = 1;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		textureDesc.size = { m_size, m_size, 6 };
		textureDesc.mipLevelCount = specification.mipCount;
		textureDesc.usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::TextureBinding;
		m_texture = graphicsContext::GetDevice().CreateTexture(&textureDesc);

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = "CubemaptextureView";
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