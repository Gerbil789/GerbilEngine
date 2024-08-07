#include "enginepch.h"
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>

namespace Engine 
{
	constexpr char* assetsDirectory = "assets"; // TODO: Make this path configurable

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_RootDirectory = assetsDirectory;
		m_CurrentDirectory = assetsDirectory;

		m_FolderIcon = Texture2D::Create("resources/icons/folder.png");
		m_FileIcon = Texture2D::Create("resources/icons/file.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
        ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        float topBarHeight = 24;
        float bottomBarHeight = 24;
        
        float availableHeight = ImGui::GetContentRegionAvail().y - (topBarHeight + bottomBarHeight);
        float windowWidth = ImGui::GetContentRegionAvail().x;

        // ----- Top Section -----
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
        ImGui::BeginChild("TopBar", ImVec2(0, topBarHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        RenderPath();
        ImGui::EndChild(); 
        ImGui::PopStyleColor();


        // ----- Main Content -----
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.5f, 0.1f, 1.0f));
        ImGui::BeginChild("MainContent", ImVec2(0, availableHeight), false);

        static float padding = 16.0f;
        static float thumbnailSize = 80;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto& path = directoryEntry.path();
            std::string filenameString = path.filename().string();

            ImGui::PushID(filenameString.c_str());
            Ref<Texture2D> icon = directoryEntry.is_directory() ? m_FolderIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

            if (ImGui::BeginDragDropSource())
            {
                std::filesystem::path relativePath(path);
                const wchar_t* itemPath = relativePath.c_str();
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                ImGui::EndDragDropSource();
            }

            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                    m_CurrentDirectory /= path.filename();
            }
            ImGui::TextWrapped(filenameString.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);
        ImGui::EndChild(); 
        ImGui::PopStyleColor();

        // ----- Bottom Section -----
        float sliderWidth = 200.0f;
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.1f, 0.5f, 1.0f));
        ImGui::BeginChild("BottomBar", ImVec2(0, bottomBarHeight));
        ImGui::SetCursorPosX((windowWidth - sliderWidth));
        ImGui::PushItemWidth(sliderWidth);
        ImGui::SliderFloat("##ThumbnailSize", &thumbnailSize, 64, 128, "%.0f");
        ImGui::PopItemWidth();
        ImGui::EndChild(); 
        ImGui::PopStyleColor();

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
				if (ImGui::Button(it->string().c_str())) { m_CurrentDirectory = m_RootDirectory; }
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
				}
			}
		}
	}

}