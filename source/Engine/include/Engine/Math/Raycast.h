#pragma once

#include <glm/glm.hpp>
#include <entt/entity/entity.hpp>

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
  };

  RayHit Raycast(const entt::registry& registry, const Ray& ray);
}