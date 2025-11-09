#pragma once

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
}