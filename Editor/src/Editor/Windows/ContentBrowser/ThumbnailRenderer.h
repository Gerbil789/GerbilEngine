#pragma once

#include <webgpu/webgpu.hpp>
#include <glm/glm.hpp>

namespace Engine { struct AssetRecord; }

namespace Editor
{
	struct Thumbnail
	{
		wgpu::TextureView view;
		glm::vec2 uv_min{ 0.0f, 0.0f };
		glm::vec2 uv_max{ 1.0f, 1.0f };
	};

	class ThumbnailRenderer
	{
	public:
		void Initialize();
		const Thumbnail& GetThumbnail(const Engine::AssetRecord& record);
	};
}