#pragma once

#include "Engine/Asset/Asset.h"
#include <webgpu/webgpu.hpp>
#include <glm/glm.hpp>

namespace Engine
{
	struct TextureSpecification
	{
		uint32_t width = 1;
		uint32_t height = 1;
		wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm;
		wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		bool generateMips = false;
	};

	class ENGINE_API Texture : public Asset
	{
	public:
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		wgpu::Texture GetTexture() const { return m_Texture; }
		wgpu::TextureView GetTextureView() const { return m_TextureView; }
		wgpu::TextureFormat GetFormat() const { return m_TextureFormat; }

	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		wgpu::Texture m_Texture;
		wgpu::TextureView m_TextureView;
		wgpu::TextureFormat m_TextureFormat;
	};

	class ENGINE_API Texture2D : public Texture
	{
	public:
		Texture2D(const TextureSpecification& specification, const void* data);
		static Texture2D* GetDefault();
		static Texture2D* GetDefaultNormal();
		static Texture2D* GetDefaultAmbient();
	};

	class ENGINE_API SubTexture2D
	{
	public:
		SubTexture2D(Texture2D* texture, const glm::vec2& min, const glm::vec2& max);

		Texture2D* GetTexture() const { return m_Texture; }
		const glm::vec2& GetUVMin() const { return m_UVMin; }
		const glm::vec2& GetUVMax() const { return m_UVMax; }

		static SubTexture2D* CreateFromGrid(Texture2D* texture, const glm::ivec2& cellCoords, const glm::ivec2& cellSize, const glm::ivec2& spriteSize = { 1,1 });

	private:
		Texture2D* m_Texture;
		glm::vec2 m_UVMin;
		glm::vec2 m_UVMax;
	};

	class ENGINE_API CubeMapTexture : public Texture
	{
	public:
		CubeMapTexture(const TextureSpecification& specification, const void* data);
		wgpu::TextureView GetPreviewView() const { return m_PreviewTextureView; }

	private:
		wgpu::TextureView m_PreviewTextureView;
	};
}