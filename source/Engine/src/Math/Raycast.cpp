#include "enginepch.h"
#include "Engine/Math/Raycast.h"
#include "Engine/Core/Components.h"
#include <entt/entity/registry.hpp>

namespace engine
{
  RayHit Raycast(const Scene& scene, const Ray& ray)
  {
    RayHit hit;
    hit.distance = (std::numeric_limits<float>::max)();

    return hit;
  }
}