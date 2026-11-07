#include "enginepch.h"
#include "Engine/System/CameraSystem.h"
#include <glm/gtx/quaternion.hpp>
#include <entt/entity/registry.hpp>

namespace engine
{
	glm::vec3 CameraSystem::GetForward(const component::Transform& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::vec3 CameraSystem::GetUp(const component::Transform& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 CameraSystem::GetRight(const component::Transform& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(1.0f, 0.0f, 0.0f));
	}

	std::array<glm::vec3, 8> CameraSystem::GetFrustumCornersWorld(float, float)
	{
		return std::array<glm::vec3, 8>();
	}

	void CameraSystem::UpdateCameraProjectionMatrix(component::CameraComponent& camera, float aspectRatio)
	{
		if(camera.projection == component::CameraComponent::Projection::Perspective)
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

	void CameraSystem::UpdateCameraViewMatrix(component::CameraComponent& camera, const component::Transform& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);

		glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);

		camera.viewMatrix = glm::lookAtLH(transform.position, transform.position + forward, up);
		camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
	}

	void CameraSystem::Update(entt::registry& registry, float aspectRatio)
	{
    auto projView = registry.view<component::CameraComponent, tag::CameraProjectionDirty>(entt::exclude<tag::DisabledTag>);
    for (auto [entity, camera] : projView.each())
    {
			UpdateCameraProjectionMatrix(camera, aspectRatio);
      registry.remove<tag::CameraProjectionDirty>(entity);
    }

		auto viewView = registry.view<component::CameraComponent, component::Transform, tag::TransformDirty>(entt::exclude<tag::DisabledTag>);
    for (auto [entity, camera, transform] : viewView.each())
    {
			UpdateCameraViewMatrix(camera, transform);
    }
	}
}

//	std::array<glm::vec3, 8> Camera::GetFrustumCornersWorld(float near, float far) const
//	{
//		glm::mat4 invView = glm::inverse(GetViewMatrix());
//
//		std::array<glm::vec3, 8> corners;
//
//		corners[0] = { glm::tan(m_perspective.fov * 0.5f) * near * m_aspectRatio, glm::tan(m_perspective.fov * 0.5f) * near, near };
//		corners[1] = { -corners[0].x, corners[0].y, corners[0].z };
//		corners[2] = { -corners[0].x, -corners[0].y, corners[0].z };
//		corners[3] = { corners[0].x, -corners[0].y, corners[0].z };
//
//		corners[4] = { glm::tan(m_perspective.fov * 0.5f) * far * m_aspectRatio, glm::tan(m_perspective.fov * 0.5f) * far, far };
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