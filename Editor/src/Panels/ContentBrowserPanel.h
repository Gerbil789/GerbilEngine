#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Renderer/Texture.h"
#include <filesystem>

namespace Engine 
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_RootDirectory;

		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;

		void RenderPath();
	};
}



