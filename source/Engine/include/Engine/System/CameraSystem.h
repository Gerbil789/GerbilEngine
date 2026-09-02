#pragma once

#include "Engine/Core/Components.h"


namespace engine
{
	class CameraSystem
	{
	public:
		static glm::vec3 GetForward(const TransformComponent& transform);
		static glm::vec3 GetUp(const TransformComponent& transform);
		static glm::vec3 GetRight(const TransformComponent& transform);
		//static void ScreenToWorldRay(const CameraComponent& camera, const WorldTransformComponent& transform, float mouseX, float mouseY, glm::vec3& outOrigin, glm::vec3& outDir);
		
		static std::array<glm::vec3, 8> GetFrustumCornersWorld(float near, float far);

		static void Update(entt::registry& registry, float aspectRatio);

		static void UpdateCameraProjectionMatrix(CameraComponent& camera, float aspectRatio);
		static void UpdateCameraViewMatrix(CameraComponent& camera, const glm::mat4& worldMatrix);
	};


}