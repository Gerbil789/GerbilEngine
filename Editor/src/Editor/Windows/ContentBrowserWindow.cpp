#include "enginepch.h"
#include "ContentBrowserWindow.h"
#include "Engine/Core/Project.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/Application.h"
#include "Editor/Core/EditorApp.h"
#include "Editor/Core/EditorIcons.h"
#include "Editor/Components/ScopedStyle.h"
#include <GLFW/glfw3.h>

#include "Engine/Asset/AssetManager.h"

namespace Editor
{
	ContentBrowserWindow::ContentBrowserWindow()
	{
		m_CurrentDirectory = Engine::Project::GetAssetsDirectory();

		m_EmptyFolderIcon = EditorIcons::GetIcon("EmptyFolder");
		m_FolderIcon = EditorIcons::GetIcon("Folder");
		m_FileIcon = EditorIcons::GetIcon("File");
		m_ImageIcon = EditorIcons::GetIcon("Image");
		m_SceneIcon = EditorIcons::GetIcon("Scene");

		glfwSetDropCallback(Engine::Application::GetWindow().GetNativeWindow(), [](GLFWwindow* window, int count, const char* paths[])
			{
				for (int i = 0; i < count; i++)
				{
					std::filesystem::path path = paths[i];
					if (std::filesystem::is_directory(path))
					{
						LOG_INFO("Dropped directory: {0}", path);
					}
					else
					{
						LOG_INFO("Dropped file: {0}", path);
					}
				}
			});


		//Engine::AssetManager

		Reload();
	}

	void ContentBrowserWindow::OnUpdate(Engine::Timestep ts)
	{
		ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		DrawNavigationBar();
		DrawMainContent();
		ImGui::End();
	}

	AssetItem LoadDirectory(const std::filesystem::path& dir)
	{
		AssetItem node(dir);

		for (auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.is_directory())
			{
				node.Children.push_back(LoadDirectory(entry.path()));
			}
			else
			{
				AssetItem file(entry.path());
				node.Children.push_back(file);
			}
		}

		return node;
	}


	void ContentBrowserWindow::Reload()
	{
		m_Items.clear();
		AssetItem root = LoadDirectory(m_CurrentDirectory);
		m_Items = root.Children;
	}

	void ContentBrowserWindow::FilterItems()
	{
	}


	void FilterAssetTree(const AssetItem& node, const std::string& search, std::vector<const AssetItem*>& outItems)
	{
		if (!node.IsDirectory && node.Name.find(search) != std::string::npos)
			outItems.push_back(&node);

		for (const auto& child : node.Children)
			FilterAssetTree(child, search, outItems);
	}

	void ContentBrowserWindow::UpdateLayoutSizes(float avail_width)
	{
		LayoutItemSpacing = (float)IconSpacing;

		m_LayoutItemSize = ImVec2(floorf(IconSize), floorf(IconSize) + 20.0f);
		m_LayoutColumnCount = std::max((int)(avail_width / (m_LayoutItemSize.x + LayoutItemSpacing)), 1);
		LayoutLineCount = (static_cast<int>(m_Items.size()) + m_LayoutColumnCount - 1) / m_LayoutColumnCount;

		m_LayoutItemStep = ImVec2(m_LayoutItemSize.x + LayoutItemSpacing, m_LayoutItemSize.y + LayoutItemSpacing);
		LayoutSelectableSpacing = std::max(floorf(LayoutItemSpacing) - IconHitSpacing, 0.0f);
		LayoutOuterPadding = floorf(LayoutItemSpacing * 0.5f);
	}

	void ContentBrowserWindow::DrawNavigationBar()
	{
		ScopedStyle style({
			{ ImGuiStyleVar_FramePadding, ImVec2(4, 4) }
			});

		ImGui::BeginChild("TopBar", ImVec2(0, m_TopBarHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::PushItemWidth(100.0f);

		if (ImGui::InputText("##Search", m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer)))
		{
			FilterItems();
		}
		ImGui::SameLine();

		DrawPath();
		ImGui::EndChild();
	}

	void ContentBrowserWindow::DrawPath()
	{
		auto relativePath = std::filesystem::relative(m_CurrentDirectory, Engine::Project::GetAssetsDirectory());

		std::filesystem::path pathSoFar = Engine::Project::GetAssetsDirectory();

		if (ImGui::Button("Assets"))
		{
			m_CurrentDirectory = Engine::Project::GetAssetsDirectory();
			Reload();
		}

		if (m_CurrentDirectory == Engine::Project::GetAssetsDirectory())
		{
			return;
		}

		// Draw remaining path components
		for (const auto& component : relativePath)
		{
			ImGui::SameLine();
			ImGui::Text("/");
			ImGui::SameLine();

			pathSoFar /= component;

			if (ImGui::Button(component.string().c_str()))
			{
				m_CurrentDirectory = pathSoFar;
				Reload();
			}
		}
	}



	void ContentBrowserWindow::ContentBrowserContextMenu()
	{
		if (ImGui::BeginPopupContextWindow("ContentBrowserContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					//create a new folder
					std::string folderName = "newFolder";
					std::filesystem::path newFolderPath = m_CurrentDirectory / folderName;
					std::filesystem::create_directory(newFolderPath);
					Reload();
				}

				ImGui::Separator();

				ImGui::EndMenu(); // End of "Create" menu

			}

			ImGui::EndPopup();
		}
	}

	void ContentBrowserWindow::ItemContextMenu()
	{
		if (ImGui::BeginPopupContextItem("ItemContextMenu"))
		{
			if (ImGui::MenuItem("Delete", "Del", false, m_Selection.Size > 0))
			{
				RequestDelete = true;
			}

			ImGui::EndPopup();
		}
	}

	

	void ContentBrowserWindow::DrawMainContent()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::BeginChild("MainContent", ImVec2(0, ImGui::GetContentRegionAvail().y - m_TopBarHeight), false);

		UpdateLayoutSizes(ImGui::GetContentRegionAvail().x);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 start_pos = ImGui::GetCursorScreenPos();
		start_pos = ImVec2(start_pos.x + LayoutOuterPadding, start_pos.y + LayoutOuterPadding);
		ImGui::SetCursorScreenPos(start_pos);

		ImGuiMultiSelectFlags ms_flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_BoxSelect2d | ImGuiMultiSelectFlags_SelectOnClickRelease;
		ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(ms_flags, m_Selection.Size, static_cast<int>(m_Items.size()));

		// Use custom selection adapter: store ID in selection (recommended)
		m_Selection.UserData = this;
		m_Selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self_, int idx) { ContentBrowserWindow* self = (ContentBrowserWindow*)self_->UserData; return self->m_Items[idx].ID; };
		m_Selection.ApplyRequests(ms_io);

		const bool want_delete = (ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_Repeat) && (m_Selection.Size > 0)) || RequestDelete;
		const int item_curr_idx_to_focus = want_delete ? m_Selection.ApplyDeletionPreLoop(ms_io, static_cast<int>(m_Items.size())) : -1;
		RequestDelete = false;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(LayoutSelectableSpacing, LayoutSelectableSpacing));

		const int column_count = m_LayoutColumnCount;
		ImGuiListClipper clipper;
		clipper.Begin(LayoutLineCount, m_LayoutItemStep.y);
		if (item_curr_idx_to_focus != -1)
		{
			clipper.IncludeItemByIndex(item_curr_idx_to_focus / column_count); // Ensure focused item line is not clipped.
		}
		if (ms_io->RangeSrcItem != -1)
		{
			clipper.IncludeItemByIndex((int)ms_io->RangeSrcItem / column_count); // Ensure RangeSrc item line is not clipped.
		}
			
		while (clipper.Step())
		{
			for (int line_idx = clipper.DisplayStart; line_idx < clipper.DisplayEnd; line_idx++)
			{
				const int item_min_idx_for_current_line = line_idx * column_count;
				const int item_max_idx_for_current_line = std::min((line_idx + 1) * column_count, static_cast<int>(m_Items.size()));
				for (int item_idx = item_min_idx_for_current_line; item_idx < item_max_idx_for_current_line; ++item_idx)
				{
					AssetItem* item_data = &m_Items[item_idx];
					ImGui::PushID((int)item_data->ID);

					// Position item
					ImVec2 pos = ImVec2(start_pos.x + (item_idx % column_count) * m_LayoutItemStep.x, start_pos.y + line_idx * m_LayoutItemStep.y);
					ImGui::SetCursorScreenPos(pos);

					ImGui::SetNextItemSelectionUserData(item_idx);
					bool item_is_selected = m_Selection.Contains(item_data->ID);
					bool item_is_visible = ImGui::IsRectVisible(m_LayoutItemSize);
					ImGui::Selectable("##unique_id", item_is_selected, ImGuiSelectableFlags_None, ImVec2(m_LayoutItemSize.x, m_LayoutItemSize.y));

					if (ImGui::IsItemToggledSelection())
					{
						item_is_selected = !item_is_selected;
					}

					// Focus (for after deletion)
					if (item_curr_idx_to_focus == item_idx)
						ImGui::SetKeyboardFocusHere(-1);

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("ASSET_ITEM", item_data->Path.c_str(), sizeof(item_data->Path.string().c_str()));
						ImGui::Text("Dragging %s", item_data->Path.string().c_str());
						ImGui::EndDragDropSource();
					}

					if (ImGui::BeginDragDropTarget()) 
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) 
						{
							auto size = payload->DataSize;


							const char* droppedFilePath = static_cast<const char*>(payload->Data);

							LOG_INFO("Dropped file path: {0}", droppedFilePath);
							//LoadFile(droppedFilePath); // Implement LoadFile to handle the dropped file
						}
						ImGui::EndDragDropTarget();
					}

					if (!item_is_visible)
					{
						ImGui::PopID();
						break;
					}

					ImVec2 box_min(pos.x - 1, pos.y - 1);
					ImVec2 box_max(box_min.x + m_LayoutItemSize.x + 2, box_min.y + m_LayoutItemSize.y + 2);
					ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);


					auto icon = m_FolderIcon;
					if (item_data->IsDirectory && std::filesystem::is_empty(item_data->Path))
					{
						icon = m_EmptyFolderIcon;
					}
					else if(!item_data->IsDirectory)
					{
						icon = m_FileIcon;
					}

					draw_list->AddImage(
						(ImTextureID)(intptr_t)(WGPUTextureView)icon->GetTexture()->GetTextureView(),
						box_min, ImVec2(box_max.x, pos.y + box_max.x - pos.x),
						ImVec2(icon->GetUVMin().x, icon->GetUVMin().y),
						ImVec2(icon->GetUVMax().x, icon->GetUVMax().y)
					);


					draw_list->AddText(ImVec2(box_min.x + m_LayoutItemSize.x / 2 - ImGui::CalcTextSize(item_data->Name.c_str()).x / 2, box_max.y - ImGui::GetFontSize() + 10), label_col, item_data->Name.c_str());
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_NewDirectory = item_data->Path;
					}

					


					//draw_list->PushClipRect(box_min, ImVec2(box_max.x, box_max.y + ImGui::GetFontSize() + 10), !item_is_selected);
					//draw_list->AddText(ImVec2(box_min.x + LayoutItemSize.x / 2 - ImGui::CalcTextSize(item_data->Label).x / 2, box_max.y - ImGui::GetFontSize() + 10), label_col, item_data->Label);
					//draw_list->PopClipRect();


					ItemContextMenu();



					ImGui::PopID();
				}
				ContentBrowserContextMenu();
			}
		}
		clipper.End();
		ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing

		ms_io = ImGui::EndMultiSelect();
		m_Selection.ApplyRequests(ms_io);
		if (want_delete)
		{
			//TODO: unload assets & delete files
			//m_Selection.ApplyDeletionPostLoop(ms_io, m_Items, item_curr_idx_to_focus);
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
				const float hovered_item_nx = (io.MousePos.x - start_pos.x + LayoutItemSpacing * 0.5f) / m_LayoutItemStep.x;
				const float hovered_item_ny = (io.MousePos.y - start_pos.y + LayoutItemSpacing * 0.5f) / m_LayoutItemStep.y;
				const int hovered_item_idx = ((int)hovered_item_ny * m_LayoutColumnCount) + (int)hovered_item_nx;
				//ImGui::SetTooltip("%f,%f -> item %d", hovered_item_nx, hovered_item_ny, hovered_item_idx); // Move those 4 lines in block above for easy debugging

				// Zoom
				IconSize *= powf(1.1f, (float)(int)ZoomWheelAccum);
				IconSize = std::clamp(IconSize, 32.0f, 128.0f);
				ZoomWheelAccum -= (int)ZoomWheelAccum;
				UpdateLayoutSizes(ImGui::GetContentRegionAvail().x);

				// Manipulate scroll to that we will land at the same Y location of currently hovered item.
				// - Calculate next frame position of item under mouse
				// - Set new scroll position to be used in next ImGui::BeginChild() call.
				float hovered_item_rel_pos_y = ((float)(hovered_item_idx / m_LayoutColumnCount) + fmodf(hovered_item_ny, 1.0f)) * m_LayoutItemStep.y;
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

		ImGui::EndChild(); // end Main Content
	}

}