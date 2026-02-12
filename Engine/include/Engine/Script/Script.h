#pragma once

#include "Engine/Scene/Entity.h"

namespace Engine
{
  class Script
  {
  public:
    virtual ~Script() = default;

    Entity Self;

    virtual void OnCreate() {}
    virtual void OnStart() {}
    virtual void OnUpdate([[maybe_unused]] float delta) {}
    virtual void OnDestroy() {}
  };
}

