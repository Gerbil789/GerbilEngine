#pragma once

#include "Editor/Windows/IPopup.h"
#include <unordered_map>
#include <string>

namespace Editor
{
	class PopupManager
	{
	public:
		static void Register(IPopup* popup)
		{
			s_Popups[popup->GetID()] = popup;
		}

		static void Open(const char* id)
		{
			s_RequestedPopup = id;
		}

		static void Draw();


	private:
		inline static std::unordered_map<std::string, IPopup*> s_Popups;
		inline static const char* s_RequestedPopup = nullptr;
	};
}