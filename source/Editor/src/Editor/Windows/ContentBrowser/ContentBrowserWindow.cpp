#include "Editor/Windows/ContentBrowser/ContentBrowserWindow.h"
#include "Editor/Windows/ContentBrowser/ThumbnailRenderer.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Utility/File.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/FileEvent.h"
#include <imgui_internal.h>

namespace Engine
{
	class MaterialAsset;
}

namespace Editor
{
	namespace
	{
		enum class ItemInteraction { None, Clicked, DoubleClicked };
		enum class ContentBrowserItemType { Directory, Asset };

		struct ContentBrowserItem
		{
			ContentBrowserItemType Type;
			Engine::AssetType AssetType;
			std::string Name;
			std::filesystem::path Path;
			Engine::Uuid AssetId;
			bool IsEmptyDirectory = false;

			ImGuiID GetID() const
			{
				if (Type == ContentBrowserItemType::Asset)
					return static_cast<ImGuiID>(static_cast<uint64_t>(AssetId));

				return static_cast<ImGuiID>(std::hash<std::string>{}(Path.string()));
			}
		};

		ThumbnailRenderer m_ThumbnailRenderer;

		ImVec2 m_ItemSize{ 64.0f, 64.0f };
		int m_ColumnCount = 0;
		int m_LineCount = 0;
		constexpr float m_ItemSpacing = 10.0f;

		std::filesystem::path m_CurrentDirectory;

		ImGuiSelectionBasicStorage m_Selection;
		std::vector<ContentBrowserItem> m_Items;
	}

	static void RefreshDirectory()
	{
		m_Items.clear();
		m_Selection.Clear();

		std::filesystem::path relativeDir = std::filesystem::relative(m_CurrentDirectory, Engine::Project::GetActive().GetAssetsDirectory());

		const Engine::DirectoryNode* node = Engine::AssetManager::GetAssetRegistry().GetDirectoryNode(relativeDir);
		if (!node) return;

		m_Items.reserve(node->subdirectories.size() + node->assets.size());

		for (const auto& [directoryName, subDirectory] : node->subdirectories)
		{
			ContentBrowserItem item;
			item.Type = ContentBrowserItemType::Directory;
			item.Name = directoryName;
			item.Path = m_CurrentDirectory / directoryName;
			item.IsEmptyDirectory = subDirectory.subdirectories.empty() && subDirectory.assets.empty();

			m_Items.push_back(item);
		}

		for (Engine::Uuid id : node->assets)
		{
			ContentBrowserItem item;
			item.Type = ContentBrowserItemType::Asset;
			item.Path = Engine::AssetManager::GetAssetPath(id);
			item.Name = item.Path.stem().string();
			item.AssetId = id;
			item.AssetType = Engine::AssetManager::GetAssetType(id);

			m_Items.push_back(item);
		}
	}

	static void OpenDirectory(const std::filesystem::path& path)
	{
		m_CurrentDirectory = path;
		RefreshDirectory();
	}

	static void DrawNavigationBar()
	{
		ScopedStyle style
		{
			{ ImGuiStyleVar_FramePadding, ImVec2(4, 4) }
		};

		ImGui::BeginChild("NavBar", ImVec2(0, 24), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		const std::filesystem::path& relativePath = std::filesystem::relative(m_CurrentDirectory, Engine::Project::GetActive().GetAssetsDirectory());
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

		ImGui::EndChild();
	}

	static void HandleZooming(ImVec2 start_pos)
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
				int hovered_item_nx = static_cast<int>((io.MousePos.x - start_pos.x + m_ItemSpacing * 0.5f) / (m_ItemSize.x + m_ItemSpacing));
				int hovered_item_ny = static_cast<int>((io.MousePos.y - start_pos.y + m_ItemSpacing * 0.5f) / (m_ItemSize.y + m_ItemSpacing + 20.0f));
				int hovered_item_idx = (hovered_item_ny * m_ColumnCount) + hovered_item_nx;

				// Zoom
				float factor = powf(1.1f, zoomWheelAccum);
				m_ItemSize.x *= factor;
				m_ItemSize.y *= factor;
				m_ItemSize.x = std::clamp(m_ItemSize.x, 32.0f, 128.0f);
				m_ItemSize.y = std::clamp(m_ItemSize.y, 32.0f, 128.0f);
				zoomWheelAccum -= (int)zoomWheelAccum;

				float hovered_item_rel_pos_y = (static_cast<float>(hovered_item_idx / m_ColumnCount) + fmodf(static_cast<float>(hovered_item_ny), 1.0f)) * (m_ItemSize.y + m_ItemSpacing + 20.0f);
				hovered_item_rel_pos_y += ImGui::GetStyle().WindowPadding.y;
				float mouse_local_y = io.MousePos.y - ImGui::GetWindowPos().y;
				ImGui::SetScrollY(hovered_item_rel_pos_y - mouse_local_y);
			}
		}
	}

	static void ContentBrowserContextMenu()
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

	static void ItemContextMenu(const ContentBrowserItem& item)
	{
		if (ImGui::BeginPopupContextItem("ItemContextMenu"))
		{
			if (ImGui::MenuItem("Delete", "", false, m_Selection.Size > 0))
			{
				/*EditorCommandManager::DeleteEntity(item.AssetId);*/
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Rename", "", false, m_Selection.Size > 0))
			{

			}

			if (ImGui::MenuItem("Open in file explorer"))
			{
				Editor::FileDialog::OpenFileExplorer(item.Path);
			}

			ImGui::EndPopup();
		}
	}

	static ItemInteraction DrawItem(const ContentBrowserItem& item, ImDrawList* draw_list, ImVec2 pos, ImU32 label_col)
	{
		// thumbnail
		const Thumbnail& thumbnail = (item.Type == ContentBrowserItemType::Directory)
			? m_ThumbnailRenderer.GetDirectoryThumbnail(item.IsEmptyDirectory)
			: m_ThumbnailRenderer.GetThumbnail(item.AssetId, item.AssetType);


		ImVec2 icon_min = pos;
		ImVec2 icon_max = { pos.x + m_ItemSize.x, pos.y + m_ItemSize.x };
		draw_list->AddImage(thumbnail.view.Get(), icon_min, icon_max, ImVec2(thumbnail.uv_min.x, thumbnail.uv_min.y), ImVec2(thumbnail.uv_max.x, thumbnail.uv_max.y));


		// label
		ImRect label_rect = ImRect{ pos.x, icon_max.y, pos.x + m_ItemSize.x, pos.y + m_ItemSize.y + 20.0f };
		std::string displayName = item.Name;
		ImVec2 text_size = ImGui::CalcTextSize(displayName.c_str());

		if (text_size.x > m_ItemSize.x)
		{
			const float ellipsis_width = ImGui::CalcTextSize("...").x;

			while (!displayName.empty() && ImGui::CalcTextSize(displayName.c_str()).x + ellipsis_width > m_ItemSize.x)
			{
				displayName.pop_back();
			}
			displayName += "...";
			text_size = ImGui::CalcTextSize(displayName.c_str());
		}

		ImVec2 text_pos = { label_rect.Min.x + (label_rect.GetWidth() - text_size.x) * 0.5f, label_rect.Min.y + 2.0f };
		draw_list->AddText(text_pos, label_col, displayName.c_str());

		ItemInteraction interaction = ItemInteraction::None;

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", item.Name.c_str());

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				interaction = ItemInteraction::DoubleClicked;
			}
			else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				interaction = ItemInteraction::Clicked;
			}
		}

		ItemContextMenu(item);
		return interaction;
	}

	static void DrawMainContent()
	{
		m_ColumnCount = std::max(static_cast<int>(ImGui::GetContentRegionAvail().x / (m_ItemSize.x + m_ItemSpacing)), 1);
		m_LineCount = (static_cast<int>(m_Items.size()) + m_ColumnCount) / m_ColumnCount;

		constexpr float layoutOuterPadding = 5.0f;

		std::filesystem::path directoryToOpen;

		ImGui::BeginChild("MainContent");

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 startPos = ImGui::GetCursorScreenPos();
		startPos = { startPos.x + layoutOuterPadding, startPos.y + layoutOuterPadding };

		ImGui::SetCursorScreenPos(startPos);

		ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_BoxSelect2d | ImGuiMultiSelectFlags_SelectOnClickRelease;
		ImGuiMultiSelectIO* io = ImGui::BeginMultiSelect(flags, m_Selection.Size, static_cast<int>(m_Items.size()));

		m_Selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage*, int id) { return static_cast<ImGuiID>(static_cast<uint64_t>(m_Items[id].GetID())); };
		m_Selection.ApplyRequests(io);

		ImGuiListClipper clipper;
		clipper.Begin(m_LineCount, m_ItemSize.y + m_ItemSpacing + 20.0f);

		while (clipper.Step())
		{
			for (int line_id = clipper.DisplayStart; line_id < clipper.DisplayEnd; ++line_id)
			{
				const int min_id = line_id * m_ColumnCount;
				const int max_id = std::min(min_id + m_ColumnCount, static_cast<int>(m_Items.size()));

				for (int item_id = min_id; item_id < max_id; ++item_id)
				{
					const ContentBrowserItem& item = m_Items[item_id];
					ImGui::PushID(item.GetID());

					ImVec2 pos = ImVec2(startPos.x + (item_id % m_ColumnCount) * (m_ItemSize.x + m_ItemSpacing), startPos.y + line_id * (m_ItemSize.y + m_ItemSpacing + 20.0f));
					ImGui::SetCursorScreenPos(pos);

					ImGui::SetNextItemSelectionUserData(item_id);
					bool item_is_selected = m_Selection.Contains(item.GetID());
					ImGui::Selectable("", item_is_selected, ImGuiSelectableFlags_AllowOverlap, m_ItemSize);

					if (ImGui::IsRectVisible(m_ItemSize))
					{
						ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);

						ItemInteraction interaction = DrawItem(item, drawList, pos, label_col);

						if (interaction == ItemInteraction::Clicked)
						{
							if (item.Type == ContentBrowserItemType::Asset)
							{
								SelectionManager::Assets.Select(item.AssetId);
							}
						}
						else if (interaction == ItemInteraction::DoubleClicked)
						{
							if (item.Type == ContentBrowserItemType::Directory)
							{
								directoryToOpen = item.Path;
							}
							else if (item.Type == ContentBrowserItemType::Asset)
							{
								switch (item.AssetType)
								{
								case Engine::AssetType::Scene:
									EditorCommandManager::OpenScene(Engine::Scene{ item.AssetId });
									break;
								default: {}
								}
							}
						}
					}

					if (item.Type == ContentBrowserItemType::Asset)
					{
						DragDropSource<Engine::Uuid>("UUID", item.AssetId, item.Name);
					}



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

		if (!directoryToOpen.empty())
		{
			OpenDirectory(directoryToOpen);
		}
	}

	void ContentBrowserWindow::Initialize()
	{
		m_ThumbnailRenderer.Initialize();
		m_CurrentDirectory = Engine::Project::GetActive().GetAssetsDirectory();

		Engine::EventBus::Subscribe<Engine::FileAddedEvent>([](const Engine::FileAddedEvent& event)
			{
				if (event.path == m_CurrentDirectory)
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
		DrawMainContent();
		ImGui::End();
	}
}