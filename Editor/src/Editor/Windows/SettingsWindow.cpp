#include "enginepch.h"
#include "SettingsWindow.h"
#include <imgui_internal.h>

namespace Editor
{
	void SettingsWindow::OnUpdate(Engine::Timestep ts)
	{
		if(!m_IsVisible) return;

		ImGui::Begin("Settings", &m_IsVisible);

		ImGui::Text("Settings Panel");

		if (ImGui::Button("Reset default editor layout")) 
		{
			ImGui::LoadIniSettingsFromDisk("resources/layouts/default.ini");
		}

		if (ImGui::Button("Save current layout"))
		{
			ImGui::SaveIniSettingsToDisk("resources/layouts/default.ini");
		}

		ImGui::End();
	}
}