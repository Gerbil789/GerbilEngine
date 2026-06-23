#pragma once

#include "Engine/Graphics/Sprite.h"

namespace Engine { enum class AssetType; }

namespace Editor
{
	enum class Icon //TODO: add invalid icon for debugging
	{
		EmptyFolder,
		Folder,
		File,
		EmptyFile,
		Audio,
		Landscape,
		Image,
		Mesh,
		Count
	};

	class IconManager
	{
	public:
		static void Initialize();
		static Engine::Sprite& GetIcon(Icon icon);
		static Engine::Sprite& GetIcon(Engine::AssetType assetType);
	};
}
