#include "enginepch.h"
#include "Engine/System/CameraSystem.h"
#include <glm/gtx/quaternion.hpp>

namespace engine
{
	glm::vec3 CameraSystem::GetForward(const TransformComponent& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::vec3 CameraSystem::GetUp(const TransformComponent& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 CameraSystem::GetRight(const TransformComponent& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(1.0f, 0.0f, 0.0f));
	}

	//void CameraSystem::ScreenToWorldRay(const CameraComponent& camera, const WorldTransformComponent& transform, float mouseX, float mouseY, glm::vec3& outOrigin, glm::vec3& outDir)
	//{

	//}

	std::array<glm::vec3, 8> CameraSystem::GetFrustumCornersWorld(float, float)
	{
		return std::array<glm::vec3, 8>();
	}



	void CameraSystem::UpdateCameraProjectionMatrix(CameraComponent& camera, float aspectRatio)
	{
		if(camera.projectionType == CameraComponent::Projection::Perspective)
		{
			camera.projectionMatrix = glm::perspectiveLH_ZO(camera.perspective.fov, aspectRatio, camera.perspective.nearClip, camera.perspective.farClip);
		}
		else
		{
			float halfHeight = camera.orthographic.size * 0.5f;
			float halfWidth = halfHeight * aspectRatio;
			camera.projectionMatrix = glm::orthoLH_ZO(-halfWidth, halfWidth, -halfHeight, halfHeight, camera.orthographic.nearClip, camera.orthographic.farClip);
		}
		camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
	}

	void CameraSystem::UpdateCameraViewMatrix(CameraComponent& camera, const TransformComponent& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);

		glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right = orientation * glm::vec3(1.0f, 0.0f, 0.0f);

		camera.viewMatrix = glm::lookAtLH(transform.position, transform.position + forward, up);
		camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
	}

	void CameraSystem::Update(entt::registry& registry, float aspectRatio)
	{
    auto projView = registry.view<CameraComponent, CameraProjectionDirty>(entt::exclude<DisabledTag>);
    for (auto [entity, camera] : projView.each())
    {
			UpdateCameraProjectionMatrix(camera, aspectRatio);
      registry.remove<CameraProjectionDirty>(entity);
    }

		auto viewView = registry.view<CameraComponent, TransformComponent, TransformDirty>(entt::exclude<DisabledTag>);
    for (auto [entity, camera, transform] : viewView.each())
    {
			UpdateCameraViewMatrix(camera, transform);
      //registry.remove<CameraViewDirty>(entity);
    }
	}
}


//	std::array<glm::vec3, 8> Camera::GetFrustumCornersWorld(float near, float far) const
//	{
//		glm::mat4 invView = glm::inverse(GetViewMatrix());
//
//		std::array<glm::vec3, 8> corners;
//
//		corners[0] = { glm::tan(m_Perspective.fov * 0.5f) * near * m_AspectRatio, glm::tan(m_Perspective.fov * 0.5f) * near, near };
//		corners[1] = { -corners[0].x, corners[0].y, corners[0].z };
//		corners[2] = { -corners[0].x, -corners[0].y, corners[0].z };
//		corners[3] = { corners[0].x, -corners[0].y, corners[0].z };
//
//		corners[4] = { glm::tan(m_Perspective.fov * 0.5f) * far * m_AspectRatio, glm::tan(m_Perspective.fov * 0.5f) * far, far };
//		corners[5] = { -corners[4].x, corners[4].y, corners[4].z };
//		corners[6] = { -corners[4].x, -corners[4].y, corners[4].z };
//		corners[7] = { corners[4].x, -corners[4].y, corners[4].z };
//
//		for (auto& corner : corners)
//		{
//			corner = glm::vec3(invView * glm::vec4(corner, 1.0f));
//		}
//
//		return corners;
//	}
//
//	void Camera::ScreenToWorldRay(float mouseX, float mouseY, glm::vec3& outOrigin, glm::vec3& outDir) const
//	{
//		float ndcX = (2.0f * mouseX) / viewportState.width - 1.0f;
//		float ndcY = 1.0f - (2.0f * mouseY) / viewportState.height;
//
//		glm::vec4 clipNear = glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
//		glm::vec4 clipFar = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
//
//		glm::mat4 invViewProj = glm::inverse(m_ProjectionMatrix * m_ViewMatrix);
//
//		glm::vec4 worldNear = invViewProj * clipNear;
//		glm::vec4 worldFar = invViewProj * clipFar;
//
//		worldNear /= worldNear.w;
//		worldFar /= worldFar.w;
//
//		outOrigin = glm::vec3(worldNear);
//		outDir = glm::normalize(glm::vec3(worldFar - worldNear));
//	}