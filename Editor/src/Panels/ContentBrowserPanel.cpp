#include "enginepch.h"
#include "ContentBrowserPanel.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Scene/Material.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Scene/SceneManager.h"


namespace Engine 
{
	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_RootDirectory = "assets";
		m_CurrentDirectory = "assets";

        m_FolderIcon = AssetManager::GetAsset<Texture2D>("resources/icons/folder.png");
        m_EmptyFolderIcon = AssetManager::GetAsset<Texture2D>("resources/icons/folder_empty.png");
        m_FileIcon = AssetManager::GetAsset<Texture2D>("resources/icons/file.png");
        m_ImageIcon = AssetManager::GetAsset<Texture2D>("resources/icons/image.png");
        m_SceneIcon = AssetManager::GetAsset<Texture2D>("resources/icons/landscape.png");

        Reload();
	}

    void ContentBrowserPanel::UpdateLayoutSizes(float avail_width)
    {
        LayoutItemSpacing = (float)IconSpacing;

        LayoutItemSize = ImVec2(floorf(IconSize), floorf(IconSize));
        LayoutColumnCount = IM_MAX((int)(avail_width / (LayoutItemSize.x + LayoutItemSpacing)), 1);
        LayoutLineCount = (Items.Size + LayoutColumnCount - 1) / LayoutColumnCount;

        LayoutItemStep = ImVec2(LayoutItemSize.x + LayoutItemSpacing, LayoutItemSize.y + LayoutItemSpacing);
        LayoutSelectableSpacing = IM_MAX(floorf(LayoutItemSpacing) - IconHitSpacing, 0.0f);
        LayoutOuterPadding = floorf(LayoutItemSpacing * 0.5f);
    }

	void ContentBrowserPanel::OnImGuiRender()
	{
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // ----- Top Section -----
        ImGui::BeginChild("TopBar", ImVec2(0, m_TopBarHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        RenderPath();
        ImGui::EndChild(); 

        // ----- Main Content -----
        ImGui::BeginChild("MainContent", ImVec2(0, ImGui::GetContentRegionAvail().y - m_TopBarHeight), false);

        UpdateLayoutSizes(ImGui::GetContentRegionAvail().x);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 start_pos = ImGui::GetCursorScreenPos();
        start_pos = ImVec2(start_pos.x + LayoutOuterPadding, start_pos.y + LayoutOuterPadding);
        ImGui::SetCursorScreenPos(start_pos);

        ImGuiMultiSelectFlags ms_flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_ClearOnClickVoid;
        ms_flags |= ImGuiMultiSelectFlags_BoxSelect2d;
        ms_flags |= ImGuiMultiSelectFlags_SelectOnClickRelease;  // - This feature allows dragging an unselected item without selecting it (rarely used)
        ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(ms_flags, Selection.Size, Items.Size);

        // Use custom selection adapter: store ID in selection (recommended)
        Selection.UserData = this;
        Selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self_, int idx) { ContentBrowserPanel* self = (ContentBrowserPanel*)self_->UserData; return self->Items[idx].ID; };
        Selection.ApplyRequests(ms_io);

        const bool want_delete = (ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_Repeat) && (Selection.Size > 0)) || RequestDelete;
        const int item_curr_idx_to_focus = want_delete ? Selection.ApplyDeletionPreLoop(ms_io, Items.Size) : -1;
        RequestDelete = false;


        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(LayoutSelectableSpacing, LayoutSelectableSpacing));


        // Rendering parameters
        const ImU32 icon_type_overlay_colors[3] = { 0, IM_COL32(200, 70, 70, 255), IM_COL32(70, 170, 70, 255) };
        const ImU32 icon_bg_color = ImGui::GetColorU32(ImGuiCol_MenuBarBg);
        const ImVec2 icon_type_overlay_size = ImVec2(4.0f, 4.0f);


        const int column_count = LayoutColumnCount;
        ImGuiListClipper clipper;
        clipper.Begin(LayoutLineCount, LayoutItemStep.y);
        if (item_curr_idx_to_focus != -1)
            clipper.IncludeItemByIndex(item_curr_idx_to_focus / column_count); // Ensure focused item line is not clipped.
        if (ms_io->RangeSrcItem != -1)
            clipper.IncludeItemByIndex((int)ms_io->RangeSrcItem / column_count); // Ensure RangeSrc item line is not clipped.
        while (clipper.Step())
        {
            for (int line_idx = clipper.DisplayStart; line_idx < clipper.DisplayEnd; line_idx++)
            {
                const int item_min_idx_for_current_line = line_idx * column_count;
                const int item_max_idx_for_current_line = IM_MIN((line_idx + 1) * column_count, Items.Size);
                for (int item_idx = item_min_idx_for_current_line; item_idx < item_max_idx_for_current_line; ++item_idx)
                {
                    ContentBrowserItem* item_data = &Items[item_idx];
                    ImGui::PushID((int)item_data->ID);

                    // Position item
                    ImVec2 pos = ImVec2(start_pos.x + (item_idx % column_count) * LayoutItemStep.x, start_pos.y + line_idx * LayoutItemStep.y);
                    ImGui::SetCursorScreenPos(pos);

                    ImGui::SetNextItemSelectionUserData(item_idx);
                    bool item_is_selected = Selection.Contains((ImGuiID)item_data->ID);
                    bool item_is_visible = ImGui::IsRectVisible(LayoutItemSize);
                    //ImGui::ShowMetricsWindow();
                    ImGui::Selectable("", item_is_selected, ImGuiSelectableFlags_None, LayoutItemSize);

                    // Update our selection state immediately (without waiting for EndMultiSelect() requests)
                    // because we use this to alter the color of our text/icon.
                    if (ImGui::IsItemToggledSelection())
                        item_is_selected = !item_is_selected;

                    // Focus (for after deletion)
                    if (item_curr_idx_to_focus == item_idx)
                        ImGui::SetKeyboardFocusHere(-1);

                    if (ImGui::BeginDragDropSource())
                    {
                        std::filesystem::path relativePath(m_CurrentDirectory / item_data->Label);
                        const wchar_t* itemPath = relativePath.c_str();
                        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                        ImGui::EndDragDropSource();
                    }

                    if (item_is_visible)
                    {
                        ImVec2 box_min(pos.x - 1, pos.y - 1);
                        ImVec2 box_max(box_min.x + LayoutItemSize.x + 2, box_min.y + LayoutItemSize.y + 2);
                        ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);

                        switch (item_data->Type)
                        {
                            case ItemType::Directory:

                                if (std::filesystem::is_empty(m_CurrentDirectory / item_data->Label))
                                {
                                    draw_list->AddImage((ImTextureID)m_EmptyFolderIcon->GetRendererID(), box_min, box_max, ImVec2(0, 1), ImVec2(1, 0));
                                }
                                else {
                                    draw_list->AddImage((ImTextureID)m_FolderIcon->GetRendererID(), box_min, box_max, ImVec2(0, 1), ImVec2(1, 0));
                                }

                                draw_list->AddText(ImVec2(box_min.x + LayoutItemSize.x / 2 - ImGui::CalcTextSize(item_data->Label).x / 2, box_max.y - ImGui::GetFontSize() + 10), label_col, item_data->Label);
                                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                {
                                    m_NewDirectory = std::filesystem::path(item_data->Path);
                                }
							    break;

                            case ItemType::File:
                                draw_list->AddImage((ImTextureID)m_FileIcon->GetRendererID(), box_min, box_max, ImVec2(0, 1), ImVec2(1, 0));
                                draw_list->AddText(ImVec2(box_min.x + LayoutItemSize.x / 2 - ImGui::CalcTextSize(item_data->Label).x / 2, box_max.y - ImGui::GetFontSize() + 10), label_col, item_data->Label);
                                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                                {
                                    std::string extension = std::filesystem::path(item_data->Label).extension().string();

                                    if (extension == ".material")
                                    {
                                        Ref<Material> material = AssetManager::GetAsset<Material>(item_data->Path);
                                        if (!material) break;

                                        Ref<Scene> scene = SceneManager::GetCurrentScene(); //TODO: use observer pattern
                                        scene->SelectMaterial(material);

                                    }
                                }
                                break;
                        }
                        ItemContextMenu();
                    }

					

                    ImGui::PopID();
                }
                ContentBrowserContextMenu();
            }
        }
        clipper.End();
        ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing

        ms_io = ImGui::EndMultiSelect();
        Selection.ApplyRequests(ms_io);
        if (want_delete)
        {
            //TODO: unload assets & delete files
            //also handle folder/s deletion
            Selection.ApplyDeletionPostLoop(ms_io, Items, item_curr_idx_to_focus);
        }
           

        // Zooming with CTRL+Wheel
        if (ImGui::IsWindowAppearing())
            ZoomWheelAccum = 0.0f;
        if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f && ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsAnyItemActive() == false)
        {
            ZoomWheelAccum += io.MouseWheel;
            if (fabsf(ZoomWheelAccum) >= 1.0f)
            {
                // Calculate hovered item index from mouse location
                // FIXME: Locking aiming on 'hovered_item_idx' (with a cool-down timer) would ensure zoom keeps on it.
                const float hovered_item_nx = (io.MousePos.x - start_pos.x + LayoutItemSpacing * 0.5f) / LayoutItemStep.x;
                const float hovered_item_ny = (io.MousePos.y - start_pos.y + LayoutItemSpacing * 0.5f) / LayoutItemStep.y;
                const int hovered_item_idx = ((int)hovered_item_ny * LayoutColumnCount) + (int)hovered_item_nx;
                //ImGui::SetTooltip("%f,%f -> item %d", hovered_item_nx, hovered_item_ny, hovered_item_idx); // Move those 4 lines in block above for easy debugging

                // Zoom
                IconSize *= powf(1.1f, (float)(int)ZoomWheelAccum);
                IconSize = IM_CLAMP(IconSize, 32.0f, 128.0f);
                ZoomWheelAccum -= (int)ZoomWheelAccum;
                UpdateLayoutSizes(ImGui::GetContentRegionAvail().x);

                // Manipulate scroll to that we will land at the same Y location of currently hovered item.
                // - Calculate next frame position of item under mouse
                // - Set new scroll position to be used in next ImGui::BeginChild() call.
                float hovered_item_rel_pos_y = ((float)(hovered_item_idx / LayoutColumnCount) + fmodf(hovered_item_ny, 1.0f)) * LayoutItemStep.y;
                hovered_item_rel_pos_y += ImGui::GetStyle().WindowPadding.y;
                float mouse_local_y = io.MousePos.y - ImGui::GetWindowPos().y;
                ImGui::SetScrollY(hovered_item_rel_pos_y - mouse_local_y);
            }
        }

        if (!m_NewDirectory.empty())
        {
            m_CurrentDirectory = m_NewDirectory;
            m_NewDirectory.clear();
            Reload();
        }

        ImGui::EndChild(); // MainContent

        ImGui::End(); // Content Browser
	}

	void ContentBrowserPanel::RenderPath()
	{
		auto currentDirectory = m_CurrentDirectory;
		std::vector<std::filesystem::path> pathComponents;
		for (auto& component : currentDirectory) {
			pathComponents.push_back(component);
		}

		for (auto it = pathComponents.begin(); it != pathComponents.end(); ++it)
		{
			if(*it == m_RootDirectory)
			{
				if (ImGui::Button(it->string().c_str())) { m_CurrentDirectory = m_RootDirectory; Reload(); }
			}
			else
			{
				ImGui::SameLine();
				ImGui::Text(">");
				ImGui::SameLine();

				if (ImGui::Button(it->string().c_str())) 
				{ 
					currentDirectory = m_RootDirectory;

					for (auto i = std::next(pathComponents.begin()); i <= it; ++i)
					{
						currentDirectory /= *i;
					}
					m_CurrentDirectory = currentDirectory;
                    Reload();
				}
			}
		}
	}

    void ContentBrowserPanel::Reload()
    {
        Items.clear();

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto& path = directoryEntry.path();
            std::size_t hash = std::hash<std::string>{}(path.string());
            ImGuiID id = static_cast<ImGuiID>(hash); 

            if(directoryEntry.is_directory())
            {
                Items.push_back(ContentBrowserItem(id, ItemType::Directory, path));
            }
            else
            {
                Items.push_back(ContentBrowserItem(id, ItemType::File, path));
            }
        }
    }




    void ContentBrowserPanel::ContentBrowserContextMenu()
    {
        if (ImGui::BeginPopupContextWindow("ContentBrowserContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::BeginMenu("Create"))
            {
                if (ImGui::MenuItem("Folder"))
                {
                    //create a new folder
                    std::string folderName = "NewFolder";
                    std::filesystem::path newFolderPath = m_CurrentDirectory / folderName;
                    std::filesystem::create_directory(newFolderPath);
                    Reload();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Scene"))
                {
                    // Logic to create a new scene
                    //TODO: Implement
                }

                if (ImGui::MenuItem("Material"))
                {
                    std::string filename = "NewMaterial";
                    std::string path = m_CurrentDirectory.string() + "/" + filename + ".material";

                    //cheeck if file already exists
                    if (std::filesystem::exists(path))
					{
						int i = 1;
						while (std::filesystem::exists(path))
						{
							filename += std::to_string(i);
							i++;
						}
                        path = m_CurrentDirectory.string() + "/" + filename + std::to_string(i) + ".material";
					}


                    Ref<Material> newMaterial = AssetManager::CreateAsset<Material>(path);
                    if (newMaterial) 
                    {
                        std::size_t hash = std::hash<std::string>{}(path);
                        ImGuiID id = static_cast<ImGuiID>(hash);
                        Serializer::Serialize(newMaterial);
                        Items.push_back(ContentBrowserItem(id, ItemType::File, path));
                    }
                }

                ImGui::EndMenu(); // End of "Create" menu
            }

            ImGui::EndPopup();
        }
    }

    void ContentBrowserPanel::ItemContextMenu()
    {
        if (ImGui::BeginPopupContextItem("ItemContextMenu"))
        {
            if (ImGui::MenuItem("Delete", "Del", false, Selection.Size > 0))
            {
                RequestDelete = true;
            }

            ImGui::EndPopup();
        }
    }

}