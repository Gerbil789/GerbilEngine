#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Asset/AssetType.h"
#include <webgpu/webgpu_cpp.h>
#include <glm/glm.hpp>

namespace editor
{
	enum class EditorIcon { Directory, EmptyDirectory, Unknown };

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
		const Thumbnail& GetThumbnail(engine::Uuid id, engine::AssetType type);
		const Thumbnail& GetDirectoryThumbnail(bool isEmpty);
	};
}