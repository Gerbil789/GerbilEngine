#include "ContentBrowserWindow.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Scene/Scene.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Utility/File.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Windows/ContentBrowser/SelectionWithDeletion.h"
#include "Engine/Core/Project.h"
#include "ThumbnailRenderer.h"
#include "Editor/Core/IconManager.h"
#include "Engine/Graphics/Texture/TextureCube.h"
//#include <GLFW/glfw3.h>

namespace Editor
{
	namespace
	{
		ThumbnailRenderer m_Renderer;

		float m_IconSize = 64.0f;
		ImVec2 m_LayoutItemSize;
		ImVec2 m_LayoutItemStep;
		float m_LayoutOuterPadding = 0.0f;
		int m_LayoutColumnCount = 0;
		int m_LayoutLineCount = 0;
		bool m_RequestDelete = false;

		SelectionWithDeletion m_Selection;
		std::filesystem::path m_CurrentDirectory;

		std::vector<Engine::AssetRecord> m_Items;
	}

	void RefreshDirectory()
	{
		m_Items.clear();
		m_Selection.Clear();

		// find directories first
		for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			auto& path = entry.path();
			if (entry.is_directory())
			{
				if(std::filesystem::is_empty(path))
				{
					m_Items.emplace_back(Engine::Uuid{}, path, Engine::AssetType::EmptyDirectory);
				}
				else
				{
					m_Items.emplace_back(Engine::Uuid{}, path, Engine::AssetType::Directory);
				}
			}
		}

		// then find files
		for (auto& record : Engine::AssetManager::GetAssetRegistry().GetAllRecords())
		{
			if (record->path.parent_path() == m_CurrentDirectory)
			{
				m_Items.emplace_back(record->id, record->path, record->type);
			}
		}
	}

	void OpenDirectory(const std::filesystem::path& path)
	{
		m_CurrentDirectory = path;
		RefreshDirectory();
	}

	void UpdateLayoutSizes()
	{
		constexpr float itemSpacing = 10.0f;

		m_LayoutItemSize = ImVec2(m_IconSize, m_IconSize + 20.0f);
		m_LayoutColumnCount = std::max(static_cast<int>(ImGui::GetContentRegionAvail().x / (m_LayoutItemSize.x + itemSpacing)), 1);
		m_LayoutLineCount = static_cast<int>((m_Items.size()) + m_LayoutColumnCount - 1) / m_LayoutColumnCount;
		m_LayoutItemStep = ImVec2(m_LayoutItemSize.x + itemSpacing, m_LayoutItemSize.y + itemSpacing);
		m_LayoutOuterPadding = itemSpacing * 0.5f;
	}

	void DrawNavigationBar()
	{
		ScopedStyle style
		{
			{ ImGuiStyleVar_FramePadding, ImVec2(4, 4) }
		};

		ImGui::BeginChild("NavBar", ImVec2(0, 24), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		auto relativePath = std::filesystem::relative(m_CurrentDirectory, Engine::Project::GetActive()->GetAssetsDirectory());

		std::filesystem::path pathSoFar = Engine::Project::GetActive()->GetAssetsDirectory();

		if (ImGui::Button("Assets"))
		{
			OpenDirectory(Engine::Project::GetActive()->GetAssetsDirectory());
		}

		if (m_CurrentDirectory != Engine::Project::GetActive()->GetAssetsDirectory())
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

		std::string text = std::format("Selected: {}/{} items", m_Selection.Size, m_Items.size());
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;
		float regionMaxX = ImGui::GetContentRegionMax().x;
		float pos = regionMaxX - textWidth;

		ImGui::SameLine(pos);
		ImGui::TextUnformatted(text.c_str());

		ImGui::EndChild();
	}

	void HandleZooming(ImVec2 start_pos)
	{
		static float zoomWheelAccum = 0.0f;
		if (ImGui::IsWindowAppearing())
		{
			zoomWheelAccum = 0.0f;
		}
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f && ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsAnyItemActive() == false)
		{
			zoomWheelAccum += io.MouseWheel;
			if (fabsf(zoomWheelAccum) >= 1.0f)
			{
				const float hovered_item_nx = (io.MousePos.x - start_pos.x + 10.0f * 0.5f) / m_LayoutItemStep.x;
				const float hovered_item_ny = (io.MousePos.y - start_pos.y + 10.0f * 0.5f) / m_LayoutItemStep.y;
				const int hovered_item_idx = ((int)hovered_item_ny * m_LayoutColumnCount) + (int)hovered_item_nx;

				// Zoom
				m_IconSize *= powf(1.1f, (float)(int)zoomWheelAccum);
				m_IconSize = std::clamp(m_IconSize, 32.0f, 128.0f);
				zoomWheelAccum -= (int)zoomWheelAccum;
				UpdateLayoutSizes();

				float hovered_item_rel_pos_y = ((float)(hovered_item_idx / m_LayoutColumnCount) + fmodf(hovered_item_ny, 1.0f)) * m_LayoutItemStep.y;
				hovered_item_rel_pos_y += ImGui::GetStyle().WindowPadding.y;
				float mouse_local_y = io.MousePos.y - ImGui::GetWindowPos().y;
				ImGui::SetScrollY(hovered_item_rel_pos_y - mouse_local_y);
			}
		}
	}

	void ProcessDragAndDrop(Engine::AssetRecord* record)
	{
		if (ImGui::BeginDragDropSource())
		{
			Engine::Uuid uuid = record->id;
			ImGui::SetDragDropPayload("UUID", &uuid, sizeof(uuid));
			ImGui::Text("%s", record->path.filename().string().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
			{
				Engine::Uuid droppedUUID = *static_cast<const Engine::Uuid*>(payload->Data);
				auto path = Engine::AssetManager::GetAssetRegistry().GetPath(droppedUUID);

				LOG_INFO("Dropped file path: {} onto {}", path, record->path.filename().string().c_str());
			}
			ImGui::EndDragDropTarget();
		}
	}

	void ContentBrowserContextMenu()
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

				//if (ImGui::MenuItem("Material"))
				//{
				//	Engine::Materials::CreateMaterial(m_CurrentDirectory / "newMaterial.mat"); //TODO: this is just bad
				//	RefreshDirectory();
				//}

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

	void ItemContextMenu()
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

	bool DrawItem(const Engine::AssetRecord& record, ImDrawList* draw_list, const ImVec2& pos, const ImU32 label_col)
	{
		ImVec2 box_min(pos.x - 1, pos.y - 1);
		ImVec2 box_max(box_min.x + m_LayoutItemSize.x + 2, box_min.y + m_LayoutItemSize.y + 2);
		ImVec2 uv_min(0.0f, 0.0f);
		ImVec2 uv_max(1.0f, 1.0f);
		wgpu::TextureView view;
		bool openRequested = false;

		switch (record.type)
		{
		case Engine::AssetType::Texture2D:
			view = Engine::AssetManager::GetAsset<Engine::Texture2D>(record.id).GetTextureView(); //TODO: use downscaled texture for thumbnail
			break;
		//case Engine::AssetType::CubeMap:
		//	view = Engine::g_AssetManager->GetAsset<Engine::TextureCube>(record.id)->GetPreviewView(); //TODO: use downscaled texture for thumbnail
		//	break;
		case Engine::AssetType::Material:
			view = m_Renderer.GetThumbnail(record.id);
			break;
		default:
			auto icon = IconManager::GetIcon(record.type);

			if (icon)
			{
				view = icon->GetTexture()->GetTextureView();
				uv_min = ImVec2(icon->GetUVMin().x, icon->GetUVMin().y);
				uv_max = ImVec2(icon->GetUVMax().x, icon->GetUVMax().y);
			}
			break;
		}

		draw_list->AddImage(
			static_cast<ImTextureRef>(static_cast<WGPUTextureView>(view)),
			box_min, ImVec2(box_max.x, pos.y + box_max.x - pos.x),
			uv_min, uv_max
		);


		// label
		const float padding = 4.0f;
		const float textHeight = ImGui::GetFontSize();
		ImVec2 labelMin { box_min.x + padding, box_max.y - textHeight };
		ImVec2 labelMax { box_max.x - padding, box_max.y };

		draw_list->PushClipRect(labelMin, labelMax, true);

		draw_list->AddText(
			ImVec2(box_min.x + m_LayoutItemSize.x / 2 - ImGui::CalcTextSize(record.path.filename().string().c_str()).x / 2, box_max.y - ImGui::GetFontSize()),
			label_col,
			record.path.filename().string().c_str());

		draw_list->PopClipRect();

		if (record.type == Engine::AssetType::Directory && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			openRequested = true;
		}

		ItemContextMenu();
		return openRequested;
	}

	std::optional<std::filesystem::path> DrawMainContent()
	{
		std::optional<std::filesystem::path> nextDirectory;

		ImGui::BeginChild("MainContent");

		UpdateLayoutSizes();

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 startPos = ImGui::GetCursorScreenPos();
		startPos = { startPos.x + m_LayoutOuterPadding, startPos.y + m_LayoutOuterPadding };

		ImGui::SetCursorScreenPos(startPos);

		ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_BoxSelect2d | ImGuiMultiSelectFlags_SelectOnClickRelease;
		ImGuiMultiSelectIO* io = ImGui::BeginMultiSelect(flags, m_Selection.Size, static_cast<int>(m_Items.size()));

		// Use custom selection adapter: store ID in selection
		m_Selection.UserData = nullptr;
		m_Selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage*, int idx) { return static_cast<unsigned int>(static_cast<uint64_t>(m_Items[idx].id)); };
		m_Selection.ApplyRequests(io);

		m_RequestDelete |= (ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_Repeat) && (m_Selection.Size > 0));
		const int item_curr_idx_to_focus = m_RequestDelete ? m_Selection.ApplyDeletionPreLoop(io, static_cast<int>(m_Items.size())) : -1;

		ImGuiListClipper clipper;
		clipper.Begin(m_LayoutLineCount, m_LayoutItemStep.y);

		if (item_curr_idx_to_focus != -1)
		{
			clipper.IncludeItemByIndex(item_curr_idx_to_focus / m_LayoutColumnCount); // Ensure focused item line is not clipped.
		}

		if (io->RangeSrcItem != -1)
		{
			clipper.IncludeItemByIndex(static_cast<int>(io->RangeSrcItem) / m_LayoutColumnCount); // Ensure RangeSrc item line is not clipped.
		}

		while (clipper.Step())
		{
			for (int line_idx = clipper.DisplayStart; line_idx < clipper.DisplayEnd; line_idx++)
			{
				const int item_min_idx_for_current_line = line_idx * m_LayoutColumnCount;
				const int item_max_idx_for_current_line = std::min((line_idx + 1) * m_LayoutColumnCount, static_cast<int>(m_Items.size()));

				for (int item_idx = item_min_idx_for_current_line; item_idx < item_max_idx_for_current_line; ++item_idx)
				{
					Engine::AssetRecord* assetRecord = &m_Items[item_idx];
					ImGui::PushID(static_cast<unsigned int>(static_cast<uint64_t>(assetRecord->id)));

					// Position item
					ImVec2 pos = ImVec2(startPos.x + (item_idx % m_LayoutColumnCount) * m_LayoutItemStep.x, startPos.y + line_idx * m_LayoutItemStep.y);
					ImGui::SetCursorScreenPos(pos);

					ImGui::SetNextItemSelectionUserData(item_idx);
					bool item_is_selected = m_Selection.Contains(static_cast<unsigned int>(static_cast<uint64_t>(assetRecord->id)));
					bool item_is_visible = ImGui::IsRectVisible(m_LayoutItemSize);
					ImGui::Selectable("##unique_id", item_is_selected, ImGuiSelectableFlags_None, m_LayoutItemSize);

					if (ImGui::IsItemToggledSelection())
					{
						item_is_selected = !item_is_selected;

						if (item_is_selected && assetRecord->type != Engine::AssetType::Directory)
						{
							// User selected this asset
							SelectionManager::Select(SelectionType::Asset, assetRecord->id);
						}
					}

					if (item_curr_idx_to_focus == item_idx)
					{
						ImGui::SetKeyboardFocusHere(-1);
					}

					ProcessDragAndDrop(assetRecord);

					if (item_is_visible)
					{
						ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);
						DrawItem(*assetRecord, drawList, pos, label_col);

						if (DrawItem(*assetRecord, drawList, pos, label_col))
						{
							nextDirectory = assetRecord->path;
						}
					}

					ImGui::PopID();
				}

			}
		}
		clipper.End();
		ContentBrowserContextMenu();

		io = ImGui::EndMultiSelect();
		m_Selection.ApplyRequests(io);
		if (m_RequestDelete)
		{
			m_Selection.ApplyDeletionPostLoop(io, m_Items, item_curr_idx_to_focus);
			m_RequestDelete = false;

			LOG_WARNING("Delete functionality is not implemented yet");
			//TODO: actually delete files from disk
		}

		// Zooming with CTRL+Wheel
		HandleZooming(startPos);

		ImGui::EndChild(); // end Main Content

		return nextDirectory;
	}

	void ContentBrowserWindow::Initialize()
	{
		m_Renderer.Initialize();

		m_CurrentDirectory = Engine::Project::GetActive()->GetAssetsDirectory();
		RefreshDirectory();

		/*glfwSetDropCallback(static_cast<GLFWwindow*>(Engine::Application::GetWindow().GetNativeWindow()), [](GLFWwindow*, int count, const char* paths[])
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
			});*/
	}

	void ContentBrowserWindow::Draw()
	{
		ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		DrawNavigationBar();
		std::optional<std::filesystem::path> nextDir = DrawMainContent();
		ImGui::End();

		if (nextDir.has_value())
		{
			OpenDirectory(nextDir.value());
		}
	}
}