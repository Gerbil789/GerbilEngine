#pragma once

#include "Engine/Core/Components.h"

namespace engine
{
	class CameraSystem
	{
	public:
		static glm::vec3 GetForward(const component::Transform& transform);
		static glm::vec3 GetUp(const component::Transform& transform);
		static glm::vec3 GetRight(const component::Transform& transform);
		//static void ScreenToWorldRay(const CameraComponent& camera, const WorldTransformComponent& transform, float mouseX, float mouseY, glm::vec3& outOrigin, glm::vec3& outDir);
		
		static std::array<glm::vec3, 8> GetFrustumCornersWorld(float near, float far);

		static void Update(entt::registry& registry, float aspectRatio);

		static void UpdateCameraProjectionMatrix(component::CameraComponent& camera, float aspectRatio);
		static void UpdateCameraViewMatrix(component::CameraComponent& camera, const component::Transform& transform);
	};


}