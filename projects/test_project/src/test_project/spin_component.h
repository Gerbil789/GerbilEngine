#pragma once

#include "engine/core/components.h"
#include "engine/core/time.h"
#include <entt/entity/registry.hpp>

struct SpinComponent
{
	float speed = 1.0f;
};

struct Spinsystem
{
  static void Update(entt::registry& registry)
  {
    auto view = registry.view<engine::component::Transform, SpinComponent>();

    for(auto[entity, transform, spin] : view.each())
    {
      transform.rotation.y += spin.speed * engine::Time::DeltaTime();
      registry.emplace_or_replace<engine::tag::TransformDirty>(entity);
    }
  }
};