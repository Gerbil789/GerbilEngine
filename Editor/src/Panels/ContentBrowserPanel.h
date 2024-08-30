#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Renderer/Texture.h"
#include <filesystem>
#include <imgui/imgui.h>

#define IM_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B) (((A) > (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

namespace Engine
{
	//TODO: move to some imgui helper file (and remove these #defines)
	struct ExampleSelectionWithDeletion : ImGuiSelectionBasicStorage
	{
		// Find which item should be Focused after deletion.
		// Call _before_ item submission. Retunr an index in the before-deletion item list, your item loop should call SetKeyboardFocusHere() on it.
		// The subsequent ApplyDeletionPostLoop() code will use it to apply Selection.
		// - We cannot provide this logic in core Dear ImGui because we don't have access to selection data.
		// - We don't actually manipulate the ImVector<> here, only in ApplyDeletionPostLoop(), but using similar API for consistency and flexibility.
		// - Important: Deletion only works if the underlying ImGuiID for your items are stable: aka not depend on their index, but on e.g. item id/ptr.
		// FIXME-MULTISELECT: Doesn't take account of the possibility focus target will be moved during deletion. Need refocus or scroll offset.
		int ApplyDeletionPreLoop(ImGuiMultiSelectIO* ms_io, int items_count)
		{
			if (Size == 0)
				return -1;

			// If focused item is not selected...
			const int focused_idx = (int)ms_io->NavIdItem;  // Index of currently focused item
			if (ms_io->NavIdSelected == false)  // This is merely a shortcut, == Contains(adapter->IndexToStorage(items, focused_idx))
			{
				ms_io->RangeSrcReset = true;    // Request to recover RangeSrc from NavId next frame. Would be ok to reset even when NavIdSelected==true, but it would take an extra frame to recover RangeSrc when deleting a selected item.
				return focused_idx;             // Request to focus same item after deletion.
			}

			// If focused item is selected: land on first unselected item after focused item.
			for (int idx = focused_idx + 1; idx < items_count; idx++)
				if (!Contains(GetStorageIdFromIndex(idx)))
					return idx;

			// If focused item is selected: otherwise return last unselected item before focused item.
			for (int idx = IM_MIN(focused_idx, items_count) - 1; idx >= 0; idx--)
				if (!Contains(GetStorageIdFromIndex(idx)))
					return idx;

			return -1;
		}

		// Rewrite item list (delete items) + update selection.
		// - Call after EndMultiSelect()
		// - We cannot provide this logic in core Dear ImGui because we don't have access to your items, nor to selection data.
		template<typename ITEM_TYPE>
		void ApplyDeletionPostLoop(ImGuiMultiSelectIO* ms_io, ImVector<ITEM_TYPE>& items, int item_curr_idx_to_select)
		{
			// Rewrite item list (delete items) + convert old selection index (before deletion) to new selection index (after selection).
			// If NavId was not part of selection, we will stay on same item.
			ImVector<ITEM_TYPE> new_items;
			new_items.reserve(items.Size - Size);
			int item_next_idx_to_select = -1;
			for (int idx = 0; idx < items.Size; idx++)
			{
				if (!Contains(GetStorageIdFromIndex(idx)))
					new_items.push_back(items[idx]);
				if (item_curr_idx_to_select == idx)
					item_next_idx_to_select = new_items.Size - 1;
			}
			items.swap(new_items);

			// Update selection
			Clear();
			if (item_next_idx_to_select != -1 && ms_io->NavIdSelected)
				SetItemSelected(GetStorageIdFromIndex(item_next_idx_to_select), true);
		}
	};


	enum class ItemType
	{
		Directory = 0,
		File = 1
	};

	struct ContentBrowserItem
	{
		ImGuiID ID;
		ItemType Type;
		char Path[256];
		char Label[32];

		ContentBrowserItem(ImGuiID id, ItemType type, std::filesystem::path path)
		{
			ID = id;
			Type = type;

			strncpy(Path, path.string().c_str(), sizeof(Path) - 1);
			Path[sizeof(Path) - 1] = '\0'; // Ensure null-termination
			
			strncpy(Label, path.filename().string().c_str(), sizeof(Label) - 1);
			Label[sizeof(Label) - 1] = '\0'; // Ensure null-termination
		}
	};


	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRender();

	private:
		void Reload();
		void UpdateLayoutSizes(float avail_width);
		void RenderPath();
		void ContentBrowserContextMenu();
		void ItemContextMenu();

	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_RootDirectory;
		std::filesystem::path m_NewDirectory = ""; // handle for switching directories

		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_EmptyFolderIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_ImageIcon;
		Ref<Texture2D> m_SceneIcon;

		ImVector<ContentBrowserItem> Items;
		ExampleSelectionWithDeletion Selection;

		const float m_TopBarHeight = 24.0f;
		const int IconHitSpacing = 4;
		const int IconSpacing = 10;
		float IconSize = 64.0f;
		ImVec2 LayoutItemSize;
		ImVec2 LayoutItemStep;
		float LayoutItemSpacing = 0.0f;
		float LayoutSelectableSpacing = 0.0f;
		float LayoutOuterPadding = 0.0f;
		int LayoutColumnCount = 0;
		int LayoutLineCount = 0;
		float ZoomWheelAccum = 0.0f;
		bool RequestDelete = false; // Deferred deletion request
	};
}