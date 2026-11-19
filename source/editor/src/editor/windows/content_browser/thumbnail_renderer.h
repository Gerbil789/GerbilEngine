#pragma once

#include "engine/asset/asset_type.h"
#include "engine/graphics/sprite.h"

namespace editor
{
	enum class Icon { Unknown = 0, Folder, EmptyFolder, texture, Text, Material, Mesh, Scene, audio};

	struct ThumbnailRenderer
	{
		void Initialize();
		const engine::Sprite& GetassetThumbnail(engine::Uuid id, engine::AssetType type);
		const engine::Sprite& GeteditorIcon(Icon icon);
	};
}