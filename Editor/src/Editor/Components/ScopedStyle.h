#pragma once

#include <initializer_list>
#include <imgui.h>

namespace Editor
{
	enum class StyleType
	{
		Var,
		Color
	};

	enum class StyleVarType
	{
		Float,
		Vec2
	};

	struct StyleEntry
	{
		StyleType type = StyleType::Var;

		// For Var
		ImGuiStyleVar var = (ImGuiStyleVar)0;
		StyleVarType varType = StyleVarType::Float;
		union
		{
			float varFloat;
			ImVec2 varVec2;
		};

		// For Color
		ImGuiCol col = (ImGuiCol)0;
		ImVec4 colValue = ImVec4(0, 0, 0, 0);

		// Default constructor ensures safe initialization
		StyleEntry() : varFloat(0.0f) {}
		StyleEntry(ImGuiStyleVar v, float val) : type(StyleType::Var), var(v), varType(StyleVarType::Float), varFloat(val), col((ImGuiCol)0), colValue(0, 0, 0, 0) {}
		StyleEntry(ImGuiStyleVar v, ImVec2 val) : type(StyleType::Var), var(v), varType(StyleVarType::Vec2), varVec2(val), col((ImGuiCol)0), colValue(0, 0, 0, 0) {}
		StyleEntry(ImGuiCol c, ImVec4 val) : type(StyleType::Color), var((ImGuiStyleVar)0), varType(StyleVarType::Float), varFloat(0.0f), col(c), colValue(val) {}
	};

	class ScopedStyle // RAII style pusher/popper
	{
	public:
		ScopedStyle(std::initializer_list<StyleEntry> entries)
		{
			for (const auto& entry : entries)
			{
				if (entry.type == StyleType::Var)
				{
					if (entry.varType == StyleVarType::Float)
					{
						ImGui::PushStyleVar(entry.var, entry.varFloat);
					}
					else if (entry.varType == StyleVarType::Vec2)
					{
						ImGui::PushStyleVar(entry.var, entry.varVec2);
					}
					m_VarCount++;
				}
				else if (entry.type == StyleType::Color)
				{
					ImGui::PushStyleColor(entry.col, entry.colValue);
					m_ColCount++;
				}
			}
		}

		~ScopedStyle()
		{
			ImGui::PopStyleVar(m_VarCount);
			ImGui::PopStyleColor(m_ColCount);
		}

	private:
		int m_VarCount = 0;
		int m_ColCount = 0;
	};
}
