#include "UIHelpers.h"

namespace Shared
{
	ScopedStyle::ScopedStyle(ImGuiStyleVar var, ImVec2 value)
	{
		ImGui::PushStyleVar(var, value); 
		count = 1;
	}

	ScopedStyle::ScopedStyle(std::initializer_list<std::pair<ImGuiStyleVar, ImVec2>> vars)
	{
		for (auto& [var, val] : vars)
		{
			ImGui::PushStyleVar(var, val);
			count++;
		}
	}

	ScopedStyle::~ScopedStyle()
	{
		ImGui::PopStyleVar(count);
	}
}

