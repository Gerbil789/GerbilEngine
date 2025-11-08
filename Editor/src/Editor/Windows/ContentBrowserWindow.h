#pragma once

#include "EditorWindow.h"
#include "Engine/Renderer/Texture.h"
#include <filesystem>
#include <imgui.h>

namespace Editor
{
	struct SelectionWithDeletion : ImGuiSelectionBasicStorage
	{
		int ApplyDeletionPreLoop(ImGuiMultiSelectIO* ms_io, int items_count)
		{
			if (Size == 0)
			{
				return -1;
			}

			// If focused item is not selected...
			const int focused_idx = (int)ms_io->NavIdItem;  // Index of currently focused item
			if (ms_io->NavIdSelected == false)  // This is merely a shortcut, == Contains(adapter->IndexToStorage(items, focused_idx))
			{
				ms_io->RangeSrcReset = true;    // Request to recover RangeSrc from NavId next frame. Would be ok to reset even when NavIdSelected==true, but it would take an extra frame to recover RangeSrc when deleting a selected item.
				return focused_idx;             // Request to focus same item after deletion.
			}

			// If focused item is selected: land on first unselected item after focused item.
			for (int idx = focused_idx + 1; idx < items_count; idx++)
			{
				if (!Contains(GetStorageIdFromIndex(idx)))
				{
					return idx;
				}

			}

			// If focused item is selected: otherwise return last unselected item before focused item.
			for (int idx = std::min(focused_idx, items_count) - 1; idx >= 0; idx--)
			{
				if (!Contains(GetStorageIdFromIndex(idx)))
				{
					return idx;
				}
			}

			return -1;
		}
	};


	struct AssetItem
	{
		ImGuiID ID; //TODO: use asset id?

		std::filesystem::path Path;
		std::string Name;
		bool IsDirectory = false;

		Ref<Engine::Texture2D> Thumbnail;

		std::vector<AssetItem> Children; // only for directories

		AssetItem(const std::filesystem::path& path) : Path(path), Name(path.filename().string()), IsDirectory(std::filesystem::is_directory(path)) 
		{
			ID = static_cast<ImGuiID>(std::hash<std::string>{}(path.string()));
		}
	};


	class ContentBrowserWindow : public EditorWindow
	{
	public:
		ContentBrowserWindow();
		void OnUpdate(Engine::Timestep ts) override;
		void Reload();

		void FilterItems();

	private:
		void UpdateLayoutSizes(float avail_width);
		void DrawPath();
		void ContentBrowserContextMenu();
		void ItemContextMenu();
		void DrawNavigationBar();
		void DrawMainContent();

	private:
		std::filesystem::path m_CurrentDirectory;
		//std::filesystem::path m_AssetsDirectory;
		std::filesystem::path m_NewDirectory = ""; // handle for switching directories

		char m_SearchBuffer[256] = { 0 };

		Ref<Engine::SubTexture2D> m_FolderIcon;
		Ref<Engine::SubTexture2D> m_EmptyFolderIcon;
		Ref<Engine::SubTexture2D> m_FileIcon;
		Ref<Engine::SubTexture2D> m_ImageIcon;
		Ref<Engine::SubTexture2D> m_SceneIcon;

		std::vector<AssetItem> m_AllItems; // cache
		std::vector<AssetItem> m_Items;
		SelectionWithDeletion m_Selection;

		const float m_TopBarHeight = 24.0f;
		const int IconHitSpacing = 4;
		const int IconSpacing = 10;
		float IconSize = 64.0f;
		ImVec2 m_LayoutItemSize;
		ImVec2 m_LayoutItemStep;
		float LayoutItemSpacing = 0.0f;
		float LayoutSelectableSpacing = 0.0f;
		float LayoutOuterPadding = 0.0f;
		int m_LayoutColumnCount = 0;
		int LayoutLineCount = 0;
		float ZoomWheelAccum = 0.0f;
		bool RequestDelete = false;

		std::vector<std::string> m_DroppedFiles;

	};
}