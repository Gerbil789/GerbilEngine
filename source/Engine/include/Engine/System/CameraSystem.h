#pragma once

#include "Engine/Core/Components.h"


namespace Engine
{
	class CameraSystem
	{
	public:
		ENGINE_API static glm::vec3 GetForward(const TransformComponent& transform);
		ENGINE_API static glm::vec3 GetUp(const TransformComponent& transform);
		ENGINE_API static glm::vec3 GetRight(const TransformComponent& transform);
		//ENGINE_API static void ScreenToWorldRay(const CameraComponent& camera, const WorldTransformComponent& transform, float mouseX, float mouseY, glm::vec3& outOrigin, glm::vec3& outDir);
		
		ENGINE_API static std::array<glm::vec3, 8> GetFrustumCornersWorld(float near, float far);

		ENGINE_API static void Update(entt::registry& registry, float aspectRatio);
	};


}