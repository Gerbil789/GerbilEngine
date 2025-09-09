#include "enginepch.h"
#include "SettingsWindow.h"
#include "Editor/Core/Core.h"
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
			ImGui::LoadIniSettingsFromDisk("Resources/Editor/layouts/default.ini");
		}

		if (ImGui::Button("Save current layout"))
		{
			ImGui::SaveIniSettingsToDisk("Resources/Editor/layouts/default.ini");
		}

		ImGui::End();
	}
}