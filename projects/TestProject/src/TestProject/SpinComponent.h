#pragma once

#include "Engine/Core/Components.h"
#include "Engine/Core/Time.h"
#include <entt/entity/registry.hpp>

struct SpinComponent
{
	float speed = 1.0f;
};

struct SpinSystem
{
  static void Update(entt::registry& registry)
  {
    auto view = registry.view<engine::TransformComponent, SpinComponent>();

    for(auto[entity, transform, spin] : view.each())
    {
      transform.rotation.y += spin.speed * engine::Time::DeltaTime();
      registry.emplace_or_replace<engine::TransformDirty>(entity);
    }
  }
};