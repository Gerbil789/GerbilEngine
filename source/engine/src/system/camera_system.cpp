#include "enginepch.h"
#include "engine/system/camera_system.h"
#include <glm/gtx/quaternion.hpp>
#include <entt/entity/registry.hpp>

namespace engine
{
	glm::vec3 Camerasystem::GetForward(const component::Transform& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::vec3 Camerasystem::GetUp(const component::Transform& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camerasystem::GetRight(const component::Transform& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);
		return glm::normalize(orientation * glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void Camerasystem::UpdateCameraProjectionMatrix(component::Camera& camera, float aspectRatio)
	{
		if (camera.projection == component::Camera::Projection::Perspective)
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

	void Camerasystem::UpdateCameraViewMatrix(component::Camera& camera, const component::Transform& transform)
	{
		glm::quat orientation = glm::quat(transform.rotation);

		glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);

		camera.viewMatrix = glm::lookAtLH(transform.position, transform.position + forward, up);
		camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
	}

	void Camerasystem::Update(entt::registry& registry, float aspectRatio)
	{
		auto projView = registry.view<component::Camera, tag::CameraProjectionDirty>(entt::exclude<tag::Disabled>);
		for (auto [entity, camera] : projView.each())
		{
			UpdateCameraProjectionMatrix(camera, aspectRatio);
			registry.remove<tag::CameraProjectionDirty>(entity);
		}

		auto viewView = registry.view<component::Camera, component::Transform, tag::TransformDirty>(entt::exclude<tag::Disabled>);
		for (auto [entity, camera, transform] : viewView.each())
		{
			UpdateCameraViewMatrix(camera, transform);
		}
	}
}