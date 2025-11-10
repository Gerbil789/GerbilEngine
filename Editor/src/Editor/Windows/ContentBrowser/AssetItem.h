#pragma once

#include "Editor/Core/EditorIcons.h"
#include "ThumbnailRenderer.h"

namespace Editor
{
	inline Icon GetIconForExtension(const std::string& ext)
	{
		static const std::unordered_map<std::string, Icon> map = {
				{".png", Icon::Image},
				{".scene", Icon::Landscape},
				{".material", Icon::File},
				{".glb", Icon::Mesh}
		};

		if (auto it = map.find(ext); it != map.end())
		{
			return it->second;
		}

		return Icon::File;
	}

	class AssetItem
	{
	public:
		Engine::UUID UUID;
		std::filesystem::path Path;
		std::string Name;
		bool IsDirectory = false;

		wgpu::TextureView Thumbnail;
		Ref<Engine::SubTexture2D> Icon;

		AssetItem(const Engine::UUID uuid, const std::filesystem::path& path) : UUID(uuid), Path(path), Name(path.filename().stem().string()), IsDirectory(std::filesystem::is_directory(path))
		{
			if (IsDirectory)
			{
				Icon = EditorIcons::GetIcon(std::filesystem::is_empty(path) ? Icon::EmptyFolder : Icon::Folder);
				return;
			}

			std::string ext = path.extension().string();
			Icon = EditorIcons::GetIcon(GetIconForExtension(ext));

			if (ext == ".png")
			{
				Thumbnail = Engine::AssetManager::GetAsset<Engine::Texture2D>(uuid)->GetTextureView();
			}
			else if (ext == ".material")
			{
				auto thumbnailRenderer = ThumbnailRenderer();
				thumbnailRenderer.Initialize();
				thumbnailRenderer.SetMaterial(Engine::AssetManager::GetAsset<Engine::Material>(uuid));
				Thumbnail = thumbnailRenderer.Render();
			}
		}
	};

}