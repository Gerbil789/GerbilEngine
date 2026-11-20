#pragma once

#include "engine/math/aabb.h"

namespace engine
{
  struct FrustumPlane
  {
    glm::vec3 normal{ 0.0f };
    float distance = 0.0f; // signed distance from origin along normal
  };

  inline FrustumPlane NormalizePlane(const glm::vec4& p)
  {
    const float invLen = 1.0f / glm::length(glm::vec3(p));
    return FrustumPlane{ glm::vec3(p) * invLen,  p.w * invLen };
  }

  inline std::array<FrustumPlane, 6> ExtractFrustumPlanesZO(const glm::mat4& vp)
  {
    const glm::vec4 r0 = glm::vec4(vp[0][0], vp[1][0], vp[2][0], vp[3][0]);
    const glm::vec4 r1 = glm::vec4(vp[0][1], vp[1][1], vp[2][1], vp[3][1]);
    const glm::vec4 r2 = glm::vec4(vp[0][2], vp[1][2], vp[2][2], vp[3][2]);
    const glm::vec4 r3 = glm::vec4(vp[0][3], vp[1][3], vp[2][3], vp[3][3]);

    return {
        NormalizePlane(r3 + r0), // Left
        NormalizePlane(r3 - r0), // Right
        NormalizePlane(r3 + r1), // Bottom
        NormalizePlane(r3 - r1), // Top
        NormalizePlane(r2),      // Near (_ZO: z >= 0)
        NormalizePlane(r3 - r2)  // Far  (_ZO: z <= w)
    };
  }

  inline bool IsAABBInFrustum(AABB aabb, const std::array<FrustumPlane, 6>& planes)
  {
    for (const auto& plane : planes)
    {
      const glm::vec3 pVertex{
          plane.normal.x >= 0.0f ? aabb.max.x : aabb.min.x,
          plane.normal.y >= 0.0f ? aabb.max.y : aabb.min.y,
          plane.normal.z >= 0.0f ? aabb.max.z : aabb.min.z
      };

      if (glm::dot(plane.normal, pVertex) + plane.distance < 0.0f)
      {
        return false; // completely outside this plane
      }
    }
    return true; // inside or intersecting all 6 planes
  }

}