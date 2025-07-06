#pragma once

#include <imgui.h>
#include <initializer_list>
#include <utility>


enum class StyleType
{
  Var,
  Color
};

struct StyleEntry
{
  StyleType type;
  union
  {
    ImGuiStyleVar var;
    ImGuiCol col;
  };
  union
  {
    ImVec2 varValue;
    ImVec4 colValue;
  };

  // Constructors
  StyleEntry(ImGuiStyleVar v, ImVec2 val)
    : type(StyleType::Var), var(v), varValue(val) {
  }

  StyleEntry(ImGuiCol c, ImVec4 val)
    : type(StyleType::Color), col(c), colValue(val) {
  }
};


class ScopedStyle
{
public:
  ScopedStyle(std::initializer_list<StyleEntry> entries);
  ~ScopedStyle();

private:
  int varCount = 0;
  int colCount = 0;
};


