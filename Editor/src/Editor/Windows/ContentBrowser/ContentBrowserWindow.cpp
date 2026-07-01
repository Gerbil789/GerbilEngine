#include "ContentBrowserWindow.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Scene/Scene.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Editor/Utility/File.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Core/Project.h"
#include "ThumbnailRenderer.h"
#include "Editor/Core/IconManager.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/FileEvent.h"

namespace Editor
{
	namespace
	{
		ThumbnailRenderer m_ThumbnailRenderer;

		float m_IconSize = 64.0f;
		ImVec2 m_LayoutItemSize;
		ImVec2 m_LayoutItemStep;
		float m_LayoutOuterPadding = 0.0f;
		int m_LayoutColumnCount = 0;
		int m_LayoutLineCount = 0;

		std::filesystem::path m_CurrentDirectory;

		ImGuiSelectionBasicStorage m_Selection;
		std::vector<Engine::AssetRecord> m_Records;

		enum class ItemInteraction
		{
			None,
			Clicked,
			DoubleClicked
		};
	}

	void RefreshDirectory()
	{
		m_Records.clear();
		m_Selection.Clear();

		// find directories first
		for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			auto& path = entry.path();
			if (entry.is_directory())
			{
				if (std::filesystem::is_empty(path))
				{
					m_Records.emplace_back(Engine::Uuid::Generate(), path, Engine::AssetType::EmptyDirectory); //TODO: generating random uuids is not ideal...
				}
				else
				{
					m_Records.emplace_back(Engine::Uuid::Generate(), path, Engine::AssetType::Directory);
				}
			}
		}
		
		// then find assets
		Engine::AssetManager::GetAssetRegistry().ForEachRecord([&](const Engine::AssetRecord& record)
		{
			if (record.path.parent_path() == m_CurrentDirectory)
			{
				m_Records.emplace_back(record.id, record.path, record.type);
			}
		});
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
		m_LayoutLineCount = (static_cast<int>(m_Records.size()) + m_LayoutColumnCount - 1) / m_LayoutColumnCount;
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

		auto relativePath = std::filesystem::relative(m_CurrentDirectory, Engine::Project::GetActive().GetAssetsDirectory());

		std::filesystem::path pathSoFar = Engine::Project::GetActive().GetAssetsDirectory();

		if (ImGui::Button("Assets"))
		{
			OpenDirectory(Engine::Project::GetActive().GetAssetsDirectory());
		}

		if (m_CurrentDirectory != Engine::Project::GetActive().GetAssetsDirectory())
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

		{
			std::string text = std::format("Selected: {}/{} items", m_Selection.Size, m_Records.size());
			float textWidth = ImGui::CalcTextSize(text.c_str()).x;
			float pos = ImGui::GetContentRegionMax().x - textWidth;
			ImGui::SameLine(pos);
			ImGui::TextUnformatted(text.c_str());
		}

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
					Engine::AssetManager::CreateAsset<Engine::Scene>(m_CurrentDirectory / "newScene.scene");
					RefreshDirectory();
				}

				if (ImGui::MenuItem("Material"))
				{
					Engine::AssetManager::CreateAsset<Engine::Material>(m_CurrentDirectory / "material.mat");
					RefreshDirectory();
				}

				ImGui::EndMenu();// End Create Menu
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Open in file explorer"))
			{
				Editor::FileDialog::OpenFileExplorer(m_CurrentDirectory);
			}

			ImGui::EndPopup();
		}
	}

	void ItemContextMenu()
	{
		if (ImGui::BeginPopupContextItem("ItemContextMenu"))
		{
			//if (ImGui::MenuItem("Delete", "", false, m_Selection.Size > 0))
			//{
			//	//m_RequestDelete = true;
			//}

			//ImGui::Separator();

			//if (ImGui::MenuItem("Rename", "", false, m_Selection.Size > 0))
			//{
			//	//TODO: Implement rename functionality
			//}

			if (ImGui::MenuItem("Open in file explorer"))
			{
				Editor::FileDialog::OpenFileExplorer(m_CurrentDirectory);
			}

			ImGui::EndPopup();
		}
	}

	ItemInteraction DrawItem(const Engine::AssetRecord& record, ImDrawList* draw_list, const ImVec2& pos, const ImU32 label_col)
	{
		ImVec2 box_min(pos.x - 1, pos.y - 1);
		ImVec2 box_max(box_min.x + m_LayoutItemSize.x + 2, box_min.y + m_LayoutItemSize.y + 2);
		ImVec2 uv_min(0.0f, 0.0f);
		ImVec2 uv_max(1.0f, 1.0f);
		wgpu::TextureView view;

		switch (record.type)
		{
		case Engine::AssetType::Texture2D:
			view = Engine::AssetManager::GetAsset<Engine::Texture2D>(record.id).GetTextureView();
			break;
		case Engine::AssetType::Material:
			view = m_ThumbnailRenderer.GetThumbnail(record.id);
			break;
		default:
			Engine::Sprite& icon = IconManager::GetIcon(record.type);

			view = Engine::AssetManager::GetAsset<Engine::Texture2D>(icon.GetTexture()).GetTextureView();
			uv_min = ImVec2(icon.GetUVMin().x, icon.GetUVMin().y);
			uv_max = ImVec2(icon.GetUVMax().x, icon.GetUVMax().y);
			break;
		}

		draw_list->AddImage(ImTextureRef{ static_cast<WGPUTextureView>(view) }, box_min, ImVec2(box_max.x, pos.y + box_max.x - pos.x), uv_min, uv_max);

		// label
		const float padding = 4.0f;
		const float textHeight = ImGui::GetFontSize();
		ImVec2 labelMin{ box_min.x + padding, box_max.y - textHeight };
		ImVec2 labelMax{ box_max.x - padding, box_max.y };

		draw_list->PushClipRect(labelMin, labelMax, true);

		draw_list->AddText(
			ImVec2(box_min.x + m_LayoutItemSize.x / 2 - ImGui::CalcTextSize(record.path.filename().string().c_str()).x / 2, box_max.y - ImGui::GetFontSize()),
			label_col,
			record.path.filename().string().c_str());

		draw_list->PopClipRect();


		ItemInteraction interaction = ItemInteraction::None;

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				interaction = ItemInteraction::DoubleClicked;
			}
			// Use IsMouseReleased rather than IsMouseClicked to prevent firing a click when the user is just initiating a Drag-and-Drop
			else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				interaction = ItemInteraction::Clicked;
			}
		}

		ItemContextMenu();
		return interaction;
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
		ImGuiMultiSelectIO* io = ImGui::BeginMultiSelect(flags, m_Selection.Size, static_cast<int>(m_Records.size()));

		m_Selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage*, int id) { return static_cast<ImGuiID>(static_cast<uint64_t>(m_Records[id].id)); };
		m_Selection.ApplyRequests(io);

		ImGuiListClipper clipper;
		clipper.Begin(m_LayoutLineCount, m_LayoutItemStep.y);

		while (clipper.Step())
		{
			for (int line_id = clipper.DisplayStart; line_id < clipper.DisplayEnd; ++line_id)
			{
				const int min_id = line_id * m_LayoutColumnCount;
				const int max_id = std::min(min_id + m_LayoutColumnCount, static_cast<int>(m_Records.size()));

				for (int item_id = min_id; item_id < max_id; ++item_id)
				{
					const Engine::AssetRecord& assetRecord = m_Records[item_id];
					ImGui::PushID(static_cast<ImGuiID>(static_cast<uint64_t>(assetRecord.id)));

					ImVec2 pos = ImVec2(startPos.x + (item_id % m_LayoutColumnCount) * m_LayoutItemStep.x, startPos.y + line_id * m_LayoutItemStep.y);
					ImGui::SetCursorScreenPos(pos);

					ImGui::SetNextItemSelectionUserData(item_id);
					bool item_is_selected = m_Selection.Contains(static_cast<ImGuiID>(static_cast<uint64_t>(assetRecord.id)));
					ImGui::Selectable("", item_is_selected, ImGuiSelectableFlags_AllowOverlap, m_LayoutItemSize);

					if (ImGui::IsRectVisible(m_LayoutItemSize))
					{
						ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);

						ItemInteraction interaction = DrawItem(assetRecord, drawList, pos, label_col);

						if (interaction == ItemInteraction::Clicked)
						{
							if (assetRecord.type != Engine::AssetType::Directory)
							{
								SelectionManager::Assets.Select(assetRecord.id);
							}
						}
						else if (interaction == ItemInteraction::DoubleClicked)
						{
							switch (assetRecord.type)
							{
							case Engine::AssetType::Directory:
								nextDirectory = assetRecord.path;
								break;
							case Engine::AssetType::Scene:
								Engine::SceneManager::SetActiveScene(assetRecord.id);
								break;
							case Engine::AssetType::Material:
								break;
							default:
								break;
							}
						}
					}

					DragDropSource source(assetRecord.GetName(), assetRecord.id);
					//DragDropTarget{}.AcceptAsset([record](Engine::Uuid droppedId) { LOG_INFO("Dropped: {} into {}", droppedId, record.GetName()); }, Engine::AssetType::Texture2D);

					ImGui::PopID();
				}

			}
		}
		clipper.End();

		ContentBrowserContextMenu();

		io = ImGui::EndMultiSelect();
		m_Selection.ApplyRequests(io);

		HandleZooming(startPos);

		ImGui::EndChild(); // end Main Content

		return nextDirectory;
	}

	void ContentBrowserWindow::Initialize()
	{
		m_ThumbnailRenderer.Initialize();
		m_CurrentDirectory = Engine::Project::GetActive().GetAssetsDirectory();

		Engine::EventBus::Subscribe<Engine::FileAddedEvent>([](const Engine::FileAddedEvent& event)
			{ 
				if(event.path == m_CurrentDirectory)
				{
					RefreshDirectory();
				}
				return false;
			});

		Engine::EventBus::Subscribe<Engine::FileRemovedEvent>([](const Engine::FileRemovedEvent& event)
			{
				if (event.path == m_CurrentDirectory)
				{
					RefreshDirectory();
				}
				return false;
			});

		Engine::EventBus::Subscribe<Engine::FileModifiedEvent>([](const Engine::FileModifiedEvent& event)
			{
				if (event.path == m_CurrentDirectory)
				{
					RefreshDirectory();
				}
				return false;
			});

		RefreshDirectory();
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