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
			ImGui::LoadIniSettingsFromDisk((EDITOR_RESOURCES / "layouts/default.ini").string().c_str());
		}

		if (ImGui::Button("Save current layout"))
		{
			ImGui::SaveIniSettingsToDisk((EDITOR_RESOURCES / "layouts/default.ini").string().c_str());
		}

		ImGui::End();
	}
}