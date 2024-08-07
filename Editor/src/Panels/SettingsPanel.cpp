#include "enginepch.h"
#include "SettingsPanel.h"

#include "imgui/imgui.h"

namespace Engine
{
	void SettingsPanel::OnImGuiRender()
	{
		if(!m_IsVisible) return;

		ImGui::Begin("Settings", &m_IsVisible);

		ImGui::Text("Settings Panel");

		ImGui::End();
	}
}