#pragma once

#include <webgpu/webgpu.hpp>

namespace Engine { class Material; }

namespace Editor
{
	class ThumbnailRenderer
	{
	public:
		void Initialize();
		wgpu::TextureView Render(Engine::Material* material);
	};
}
