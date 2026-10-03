#pragma once

#include "Engine/Asset/AssetType.h"
#include "Engine/Graphics/Sprite.h"

namespace editor
{
	enum class Icon { Unknown = 0, Folder, EmptyFolder, Texture, Text, Material, Mesh, Scene, Audio};

	class ThumbnailRenderer
	{
	public:
		void Initialize();

		const engine::Sprite& GetAssetThumbnail(engine::Uuid id, engine::AssetType type);
		const engine::Sprite& GetEditorIcon(Icon icon);
	};
}