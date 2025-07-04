#pragma once

#include <imgui.h>
#include <initializer_list>
#include <utility>

namespace Shared
{
	// Helper class to set style for window scope, and restore it afterwards
	class ScopedStyle
	{
	public:
		ScopedStyle(ImGuiStyleVar var, ImVec2 value);
		ScopedStyle(std::initializer_list<std::pair<ImGuiStyleVar, ImVec2>> vars);
		~ScopedStyle();

	private:
		int count = 0;
	};
}

