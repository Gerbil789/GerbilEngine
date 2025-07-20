#include "enginepch.h"
#include "Texture.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/GraphicsContext.h"
#include <stb_image.h>

namespace Engine
{
	Texture2D::Texture2D(const std::filesystem::path& path) : Asset(path)
	{
		int width, height;
		unsigned char* pixelData = stbi_load(path.string().c_str(), &width, &height, &m_Channels, STBI_rgb_alpha);

		if(!pixelData)
		{
			LOG_ERROR("Failed to load texture at path: {0}", path.string());
			return;
		}

		m_Width = static_cast<uint32_t>(width);
		m_Height = static_cast<uint32_t>(height);
		m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = m_TextureFormat;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { m_Width, m_Height, 1 };
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TexelCopyTextureInfo dst{};
		dst.texture = m_Texture;
		dst.mipLevel = 0;
		dst.origin = { 0, 0, 0 };
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout{};
		layout.offset = 0;
		layout.bytesPerRow = m_Width * 4; // 4 bytes per pixel (RGBA8)
		layout.rowsPerImage = m_Height;

		wgpu::Extent3D size = { m_Width, m_Height, 1 };

		GraphicsContext::GetQueue().writeTexture(dst, pixelData, m_Width * m_Height * 4, layout, size);

		stbi_image_free(pixelData);

		wgpu::TextureViewDescriptor viewDesc{};
		viewDesc.format = m_TextureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		viewDesc.aspect = wgpu::TextureAspect::All;
		m_TextureView = m_Texture.createView(viewDesc);
	}
}