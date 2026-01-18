#include "NewProjectPopupWindow.h"
#include "Engine/Utils/File.h"
#include <imgui.h>

namespace Editor
{
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
				std::string dir = Engine::OpenDirectory();
				if (!dir.empty())
				{
					strncpy(m_Path, dir.c_str(), sizeof(m_Path));
				}
			}

			if (ImGui::Button("Create"))
				ImGui::CloseCurrentPopup();

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
}