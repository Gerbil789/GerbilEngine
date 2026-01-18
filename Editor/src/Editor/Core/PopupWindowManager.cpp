#include "PopupWindowManager.h"
#include <imgui.h>

namespace Editor
{
	void PopupManager::Draw()
	{
		if (s_RequestedPopup)
		{
			ImGui::OpenPopup(s_RequestedPopup);
			s_RequestedPopup = nullptr;
		}

		for (auto& [_, popup] : s_Popups)
		{
			popup->Draw();
		}
	}
}