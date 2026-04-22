#pragma once

#include <webgpu/webgpu-raii.hpp>

namespace Engine { class Uuid; }

namespace Editor
{
	class ThumbnailRenderer
	{
	public:
		void Initialize();
		wgpu::TextureView GetThumbnail(Engine::Uuid id);
	};

}
