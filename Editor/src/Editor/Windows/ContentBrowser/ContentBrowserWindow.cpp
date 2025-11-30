#include "enginepch.h"
#include "ContentBrowserWindow.h"
#include "Engine/Core/Project.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Application.h"
#include "Editor/Components/ScopedStyle.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Utils/File.h"
#include "Editor/Core/EditorContext.h"

#include <GLFW/glfw3.h>

namespace Editor
{
	ContentBrowserWindow::ContentBrowserWindow()
	{
		m_CurrentDirectory = Engine::Project::GetAssetsDirectory();

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

		OpenDirectory(m_CurrentDirectory);
	}

	void ContentBrowserWindow::OnUpdate()
	{
		ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		DrawNavigationBar();
		DrawMainContent();
		ImGui::End();

		if (directoryToOpenIndex != -1)
		{
			auto& item = m_Items[directoryToOpenIndex];
			OpenDirectory(item.Path);
			directoryToOpenIndex = -1;
		}
	}

	void ContentBrowserWindow::OpenDirectory(const std::filesystem::path& path)
	{
		m_CurrentDirectory = path;
		RefreshDirectory();
	}

	void ContentBrowserWindow::RefreshDirectory()
	{
		m_Items.clear();
		m_Selection.Clear();

		// find directories first
		for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			auto& path = entry.path();
			if (entry.is_directory())
			{
				m_Items.emplace_back(Engine::UUID(), path); // UUID optional for folder
			}
		}

		// then find files
		for (auto& meta : Engine::AssetManager::GetAllAssetMetadata())
		{
			if (meta->path.parent_path() == m_CurrentDirectory)
			{
				m_Items.emplace_back(meta->id, meta->path);
			}
		}
	}

	void ContentBrowserWindow::UpdateLayoutSizes(float avail_width)
	{
		const float itemSpacing = 10.0f;

		m_LayoutItemSize = ImVec2(floorf(IconSize), floorf(IconSize) + 20.0f);
		m_LayoutColumnCount = std::max((int)(avail_width / (m_LayoutItemSize.x + itemSpacing)), 1);
		LayoutLineCount = (static_cast<int>(m_Items.size()) + m_LayoutColumnCount - 1) / m_LayoutColumnCount;

		m_LayoutItemStep = ImVec2(m_LayoutItemSize.x + itemSpacing, m_LayoutItemSize.y + itemSpacing);
		LayoutSelectableSpacing = std::max(floorf(itemSpacing) - 4, 0.0f);
		LayoutOuterPadding = floorf(itemSpacing * 0.5f);
	}

	void ContentBrowserWindow::DrawNavigationBar()
	{
		ScopedStyle style({
			{ ImGuiStyleVar_FramePadding, ImVec2(4, 4) }
			});

		ImGui::BeginChild("NavBar", ImVec2(0, 24), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		auto relativePath = std::filesystem::relative(m_CurrentDirectory, Engine::Project::GetAssetsDirectory());

		std::filesystem::path pathSoFar = Engine::Project::GetAssetsDirectory();

		if (ImGui::Button("Assets"))
		{
			OpenDirectory(Engine::Project::GetAssetsDirectory());
		}

		if (m_CurrentDirectory != Engine::Project::GetAssetsDirectory())
		{
			for (const auto& component : relativePath)
			{
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();

				pathSoFar /= component;

				if (ImGui::Button(component.string().c_str()))
				{
					OpenDirectory(pathSoFar);
				}
			}
		}

		ImGui::SameLine();
		char buffer[64];
		snprintf(buffer, sizeof(buffer), "Selected: %d/%d items", m_Selection.Size, (int)m_Items.size());

		float textWidth = ImGui::CalcTextSize(buffer).x;
		float regionMaxX = ImGui::GetContentRegionMax().x;
		float pos = regionMaxX - textWidth;

		ImGui::SameLine(pos);
		ImGui::TextUnformatted(buffer);

		ImGui::EndChild();
	}

	void ContentBrowserWindow::ContentBrowserContextMenu()
	{
		if (ImGui::BeginPopupContextWindow("ContentBrowserContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Directory"))
				{
					std::filesystem::create_directory(m_CurrentDirectory / "newDirectory");
					RefreshDirectory();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Scene"))
				{
					RefreshDirectory();
				}

				if (ImGui::MenuItem("Material"))
				{
					RefreshDirectory();
				}

				ImGui::EndMenu();// End Create Menu
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Open in file explorer"))
			{
				Engine::OpenFileExplorer(m_CurrentDirectory);
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
				m_RequestDelete = true;
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Open in file explorer"))
			{
				Engine::OpenFileExplorer(m_CurrentDirectory);
			}

			ImGui::EndPopup();
		}
	}

	void ContentBrowserWindow::DrawMainContent()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::BeginChild("MainContent", ImVec2(0, ImGui::GetContentRegionAvail().y), false);

		UpdateLayoutSizes(ImGui::GetContentRegionAvail().x);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 start_pos = ImGui::GetCursorScreenPos();
		start_pos = ImVec2(start_pos.x + LayoutOuterPadding, start_pos.y + LayoutOuterPadding);
		ImGui::SetCursorScreenPos(start_pos);

		ImGuiMultiSelectFlags ms_flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_BoxSelect2d | ImGuiMultiSelectFlags_SelectOnClickRelease;
		ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(ms_flags, m_Selection.Size, static_cast<int>(m_Items.size()));

		// Use custom selection adapter: store ID in selection (recommended)
		m_Selection.UserData = this;
		m_Selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self_, int idx) { ContentBrowserWindow* self = (ContentBrowserWindow*)self_->UserData; return (unsigned int)self->m_Items[idx].UUID; };
		m_Selection.ApplyRequests(ms_io);

		const bool want_delete = (ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_Repeat) && (m_Selection.Size > 0)) || m_RequestDelete;
		const int item_curr_idx_to_focus = want_delete ? m_Selection.ApplyDeletionPreLoop(ms_io, static_cast<int>(m_Items.size())) : -1;
		m_RequestDelete = false;

		const int column_count = m_LayoutColumnCount;
		ImGuiListClipper clipper;
		clipper.Begin(LayoutLineCount, m_LayoutItemStep.y);
		if (item_curr_idx_to_focus != -1)
		{
			clipper.IncludeItemByIndex(item_curr_idx_to_focus / column_count); // Ensure focused item line is not clipped.
		}
		if (ms_io->RangeSrcItem != -1)
		{
			clipper.IncludeItemByIndex(static_cast<int>(ms_io->RangeSrcItem) / column_count); // Ensure RangeSrc item line is not clipped.
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
					ImGui::PushID((unsigned int)item_data->UUID);

					// Position item
					ImVec2 pos = ImVec2(start_pos.x + (item_idx % column_count) * m_LayoutItemStep.x, start_pos.y + line_idx * m_LayoutItemStep.y);
					ImGui::SetCursorScreenPos(pos);

					ImGui::SetNextItemSelectionUserData(item_idx);
					bool item_is_selected = m_Selection.Contains((unsigned int)item_data->UUID);
					bool item_is_visible = ImGui::IsRectVisible(m_LayoutItemSize);
					ImGui::Selectable("##unique_id", item_is_selected, ImGuiSelectableFlags_None, m_LayoutItemSize);

					if (ImGui::IsItemToggledSelection())
					{
						item_is_selected = !item_is_selected;

						if (item_is_selected && !item_data->IsDirectory)
						{
							// User selected this asset
							EditorContext::SelectAsset(item_data->UUID);
						}
					}

					if (item_curr_idx_to_focus == item_idx)
					{
						ImGui::SetKeyboardFocusHere(-1);
					}

					if (ImGui::BeginDragDropSource())
					{
						Engine::UUID uuid = item_data->UUID;
						ImGui::SetDragDropPayload("UUID", &uuid, sizeof(uuid));
						ImGui::Text("%s", item_data->Path.string().c_str());
						ImGui::EndDragDropSource();
					}

					if (ImGui::BeginDragDropTarget()) 
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID")) 
						{
							Engine::UUID droppedUUID = *static_cast<const Engine::UUID*>(payload->Data);
							auto path = Engine::AssetManager::GetAssetPath(droppedUUID);
							LOG_INFO("Dropped file path: {0}", path);
						}
						ImGui::EndDragDropTarget();
					}

					if (item_is_visible)
					{
						ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);
						DrawItem(*item_data, item_idx, draw_list, pos, label_col);
					}

					ImGui::PopID();
				}

			}
		}
		clipper.End();
		ContentBrowserContextMenu();

		ms_io = ImGui::EndMultiSelect();
		m_Selection.ApplyRequests(ms_io);
		if (want_delete)
		{
			m_Selection.ApplyDeletionPostLoop(ms_io, m_Items, item_curr_idx_to_focus);
			//TODO: actually delete files from disk
		}

		// Zooming with CTRL+Wheel
		if (ImGui::IsWindowAppearing())
		{
			ZoomWheelAccum = 0.0f;
		}

		if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f && ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsAnyItemActive() == false)
		{
			ZoomWheelAccum += io.MouseWheel;
			if (fabsf(ZoomWheelAccum) >= 1.0f)
			{
				const float hovered_item_nx = (io.MousePos.x - start_pos.x + 10.0f * 0.5f) / m_LayoutItemStep.x;
				const float hovered_item_ny = (io.MousePos.y - start_pos.y + 10.0f * 0.5f) / m_LayoutItemStep.y;
				const int hovered_item_idx = ((int)hovered_item_ny * m_LayoutColumnCount) + (int)hovered_item_nx;

				// Zoom
				IconSize *= powf(1.1f, (float)(int)ZoomWheelAccum);
				IconSize = std::clamp(IconSize, 32.0f, 128.0f);
				ZoomWheelAccum -= (int)ZoomWheelAccum;
				UpdateLayoutSizes(ImGui::GetContentRegionAvail().x);

				float hovered_item_rel_pos_y = ((float)(hovered_item_idx / m_LayoutColumnCount) + fmodf(hovered_item_ny, 1.0f)) * m_LayoutItemStep.y;
				hovered_item_rel_pos_y += ImGui::GetStyle().WindowPadding.y;
				float mouse_local_y = io.MousePos.y - ImGui::GetWindowPos().y;
				ImGui::SetScrollY(hovered_item_rel_pos_y - mouse_local_y);
			}
		}

		ImGui::EndChild(); // end Main Content
	}

	void ContentBrowserWindow::DrawItem(const AssetItem& item_data, int item_idx, ImDrawList* draw_list, const ImVec2& pos, const ImU32 label_col)
	{
		ImVec2 box_min(pos.x - 1, pos.y - 1);
		ImVec2 box_max(box_min.x + m_LayoutItemSize.x + 2, box_min.y + m_LayoutItemSize.y + 2);

		if (item_data.Thumbnail != nullptr)
		{
			// thumbnail
			draw_list->AddImage(
				(ImTextureID)(intptr_t)(WGPUTextureView)item_data.Thumbnail,
				box_min, ImVec2(box_max.x, pos.y + box_max.x - pos.x),
				ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)
			);
		}
		else
		{
			// icon
			draw_list->AddImage(
				(ImTextureID)(intptr_t)(WGPUTextureView)item_data.Icon->GetTexture()->GetTextureView(),
				box_min, ImVec2(box_max.x, pos.y + box_max.x - pos.x),
				ImVec2(item_data.Icon->GetUVMin().x, item_data.Icon->GetUVMin().y),
				ImVec2(item_data.Icon->GetUVMax().x, item_data.Icon->GetUVMax().y)
			);
		}

		// label
		draw_list->AddText(ImVec2(box_min.x + m_LayoutItemSize.x / 2 - ImGui::CalcTextSize(item_data.Name.c_str()).x / 2, box_max.y - ImGui::GetFontSize() + 10), label_col, item_data.Name.c_str());

		if (item_data.IsDirectory && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			directoryToOpenIndex = item_idx;
		}

		ItemContextMenu();
	}

}