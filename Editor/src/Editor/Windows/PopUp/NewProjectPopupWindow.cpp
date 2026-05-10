#include "NewProjectPopupWindow.h"
#include "Engine/Core/Project.h"
#include "Editor/Utility/File.h"
#include <imgui.h>

namespace Editor
{
	static char m_Name[128]{};
	static char m_Path[512]{};

	void NewProjectPopupWindow::Draw()
	{
		if (m_OpenRequested)
		{
			ImGui::OpenPopup(GetID());
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f, 0.5f });
			m_OpenRequested = false;
		}

		if (ImGui::BeginPopupModal(GetID(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Name", m_Name, sizeof(m_Name));
			ImGui::InputText("Path", m_Path, sizeof(m_Path));
			ImGui::SameLine();
			if(ImGui::Button("Browse"))
			{
				std::filesystem::path dir = OpenDirectory();
				if (!dir.empty())
				{
					strncpy_s(m_Path, dir.string().c_str(), sizeof(m_Path));
				}
			}

			if (ImGui::Button("Create"))
			{
				std::filesystem::path fullPath = std::filesystem::path(m_Path) / m_Name;
				Engine::Project::New(fullPath);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}