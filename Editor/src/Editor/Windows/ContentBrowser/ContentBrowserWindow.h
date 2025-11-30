#pragma once

#include "Editor/Windows/IEditorWindow.h"
#include "AssetItem.h"
#include "ThumbnailRenderer.h"
#include "Editor/Utility/SelectionWithDeletion.h"
#include "Engine/Renderer/Texture.h"
#include <filesystem>

namespace Editor
{
	class ContentBrowserWindow : public IEditorWindow
	{
	public:
		ContentBrowserWindow();
		void OnUpdate() override;
		void OpenDirectory(const std::filesystem::path& path);
		void RefreshDirectory();

	private:
		void UpdateLayoutSizes(float avail_width);
		void ContentBrowserContextMenu();
		void ItemContextMenu();
		void DrawNavigationBar();
		void DrawMainContent();
		void DrawItem(const AssetItem& item_data, int item_idx, ImDrawList* draw_list, const ImVec2& pos, const ImU32 label_col);

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
		bool m_RequestDelete = false;
	};
}