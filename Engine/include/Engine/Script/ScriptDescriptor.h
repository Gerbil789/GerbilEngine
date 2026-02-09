#pragma once

#include "Engine/Script/Script.h"
#include "Engine/Script/ScriptFieldType.h"
#include <vector>
#include <string>

namespace Engine
{
  struct ScriptField
  {
    std::string name;
    ScriptFieldType type;
    size_t offset;
  };

  struct ScriptDescriptor
  {
    std::string name;
    std::vector<ScriptField> fields;
    std::function<Script*()> factory;
  };
}
