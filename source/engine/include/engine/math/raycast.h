#pragma once

#include "engine/core/scene.h"
#include <glm/glm.hpp>

namespace engine
{
  struct Ray
  {
    glm::vec3 origin;
    glm::vec3 direction;
  };

  struct RayHit
  {
    entt::entity entity = entt::null;
    float distance;
    glm::vec3 point;
    glm::vec3 normal;

    explicit operator bool() const { return entity != entt::null; }
  };

  RayHit Raycast(const Scene& scene, const Ray& ray);
}