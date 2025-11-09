#pragma once

#include "Editor/Windows/EditorWindow.h"
#include "AssetItem.h"
#include "ThumbnailRenderer.h"
#include "Editor/Utility/SelectionWithDeletion.h"
#include "Engine/Renderer/Texture.h"
#include <filesystem>

namespace Editor
{
	class ContentBrowserWindow : public EditorWindow
	{
	public:
		ContentBrowserWindow();
		void OnUpdate(Engine::Timestep ts) override;
		void OpenDirectory(const std::filesystem::path& path);
		void RefreshDirectory();

	private:
		void UpdateLayoutSizes(float avail_width);
		void ContentBrowserContextMenu();
		void ItemContextMenu();
		void DrawNavigationBar();
		void DrawMainContent();

	private:
		std::filesystem::path m_CurrentDirectory;
		int directoryToOpenIndex = -1;

		std::vector<AssetItem> m_Items;
		SelectionWithDeletion m_Selection;


		float IconSize = 64.0f;
		ImVec2 m_LayoutItemSize;
		ImVec2 m_LayoutItemStep;
		float LayoutSelectableSpacing = 0.0f;
		float LayoutOuterPadding = 0.0f;
		int m_LayoutColumnCount = 0;
		int LayoutLineCount = 0;
		float ZoomWheelAccum = 0.0f;
		bool RequestDelete = false;
	};
}