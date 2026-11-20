#pragma once

#include "engine/core/components.h"

namespace engine
{
	struct Camerasystem
	{
		static glm::vec3 GetForward(const component::Transform& transform);
		static glm::vec3 GetUp(const component::Transform& transform);
		static glm::vec3 GetRight(const component::Transform& transform);

		static void Update(entt::registry& registry, float aspectRatio);

		static void UpdateCameraProjectionMatrix(component::Camera& camera, float aspectRatio);
		static void UpdateCameraViewMatrix(component::Camera& camera, const component::Transform& transform);
	};
}