#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Asset.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class Texture2D : public IAsset
	{
	public:
		Texture2D(const std::filesystem::path& path);
		Texture2D() = default;
		~Texture2D() = default;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		wgpu::TextureView GetTextureView() const { return m_TextureView; }

	protected:
		int m_Channels;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		wgpu::Texture m_Texture;
		wgpu::TextureView m_TextureView;
		wgpu::TextureFormat m_TextureFormat;

	};
}