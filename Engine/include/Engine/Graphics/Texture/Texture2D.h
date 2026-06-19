#pragma once

#include "Engine/Asset/Asset.h"
#include <glm/glm.hpp>
#include <webgpu/webgpu.hpp>

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

	class ENGINE_API Texture2D : public Asset
	{
	public:
		Texture2D(const TextureSpecification& specification, const void* data);

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		wgpu::Texture GetTexture() const { return m_Texture; }
		wgpu::TextureView GetTextureView() const { return m_TextureView; }
		wgpu::TextureFormat GetFormat() const { return m_TextureFormat; }

	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		wgpu::Texture m_Texture;
		wgpu::TextureView m_TextureView;
		wgpu::TextureFormat m_TextureFormat;
	};

	//TODO: move subtexture to its own file, and probably rework it...
	class ENGINE_API SubTexture2D
	{
	public:
		SubTexture2D(Uuid texture, const glm::vec2& min, const glm::vec2& max);

		const Uuid GetTexture() const { return m_Texture; }
		const glm::vec2& GetUVMin() const { return m_UVMin; }
		const glm::vec2& GetUVMax() const { return m_UVMax; }

		static SubTexture2D CreateFromGrid(Uuid texture, const glm::ivec2& cellCoords, const glm::ivec2& cellSize, const glm::ivec2& spriteSize = { 1,1 });

	private:
		Uuid m_Texture;
		glm::vec2 m_UVMin;
		glm::vec2 m_UVMax;
	};
}