#pragma once

#include <glm/glm.hpp>

bool RayIntersectsPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& planeNormal, const glm::vec3& planeOrigin, glm::vec3& outHitPoint)
{
  //glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f); // Pointing up
  //glm::vec3 planeOrigin = glm::vec3(0.0f, 0.0f, 0.0f); // Grid center

  float denominator = glm::dot(rayDir, planeNormal);

  // If denominator is near 0, the ray is parallel to the floor.
  // If it's positive, the ray is pointing away from the floor.
  if (denominator > -0.000001f) 
  {
    return false;
  }

  float t = glm::dot(planeOrigin - rayOrigin, planeNormal) / denominator;

  outHitPoint = rayOrigin + (rayDir * t);
  return true;
}