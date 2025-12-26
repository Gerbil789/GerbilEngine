#pragma once

#include "Engine/Graphics/Material.h"

namespace Editor
{
	class ThumbnailRenderer
	{
	public:
		void Initialize();
		wgpu::TextureView Render(Engine::Material* material);
	};
}
