#pragma once

#include "Engine/Graphics/Texture/Texture.h"

namespace Engine
{
	class ENGINE_API TextureCube : public Texture
	{
	public:
		TextureCube(const TextureSpecification& specification);

		TextureCube(const TextureSpecification& specification, const void* data);
		wgpu::TextureView GetPreviewView() const { return m_PreviewTextureView; }

	private:
		wgpu::TextureView m_PreviewTextureView;
	};
}