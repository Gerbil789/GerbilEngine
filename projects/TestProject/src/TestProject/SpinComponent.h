#pragma once

#include "Engine/Core/Components.h"
#include <entt/entity/registry.hpp>

struct SpinComponent
{
	float speed = 10.0f;
};

class SpinSystem
{
public:
  void update(entt::registry& registry, float delta)
  {
    auto view = registry.view<engine::TransformComponent, SpinComponent>();

    for(auto[entity, transform, spin] : view.each())
    {
      transform.rotation += spin.speed * delta;
      registry.emplace_or_replace<engine::TransformDirty>(entity);
    }
  }
};