#include "UIHelpers.h"

ScopedStyle::ScopedStyle(std::initializer_list<StyleEntry> entries)
{
  for (const auto& entry : entries)
  {
    if (entry.type == StyleType::Var)
    {
      ImGui::PushStyleVar(entry.var, entry.varValue);
      varCount++;
    }
    else if (entry.type == StyleType::Color)
    {
      ImGui::PushStyleColor(entry.col, entry.colValue);
      colCount++;
    }
  }
}

ScopedStyle::~ScopedStyle()
{
	ImGui::PopStyleVar(varCount);
	ImGui::PopStyleColor(colCount);
}


