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
		StyleType type;

		// For Var
		ImGuiStyleVar var;
		StyleVarType varType;
		union
		{
			float varFloat = 0.0f;
			ImVec2 varVec2;
		};

		// For Color
		ImGuiCol col;
		ImVec4 colValue;

		// Constructors
		StyleEntry(ImGuiStyleVar v, float val)
			: type(StyleType::Var), var(v), varType(StyleVarType::Float), varFloat(val) {
		}

		StyleEntry(ImGuiStyleVar v, ImVec2 val)
			: type(StyleType::Var), var(v), varType(StyleVarType::Vec2), varVec2(val) {
		}

		StyleEntry(ImGuiCol c, ImVec4 val)
			: type(StyleType::Color), col(c), colValue(val) {
		}
	};

	class ScopedStyle
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
