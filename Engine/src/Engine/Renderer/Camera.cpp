#include "enginepch.h"
#include "Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	void Camera::SetViewportSize(glm::vec2 size)
	{
		m_AspectRatio = size.x / size.y;
		UpdateProjectionMatrix();
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
	}

	void Camera::SetRotation(float pitch, float yaw)
	{
		m_Pitch = pitch;
		m_Yaw = yaw;
	}

	void Camera::UpdateViewMatrix()
	{
		glm::quat orientation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
		glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 zoomedPosition = m_Position - forward * m_Zoom;

		m_ViewMatrix = glm::lookAt(zoomedPosition, zoomedPosition + forward, up);
	}

	void Camera::UpdateProjectionMatrix()
	{
		switch (m_ProjectionData.Type)
		{
		case ProjectionType::Perspective:
		{
			float perspFov = m_ProjectionData.Perspective.FOV;
			float perspNear = m_ProjectionData.Perspective.Near;
			float perspFar = m_ProjectionData.Perspective.Far;
			m_ProjectionMatrix = glm::perspective(perspFov, m_AspectRatio, perspNear, perspFar);
			break;
		}
		case ProjectionType::Orthographic:
		{
			float orthoLeft = -m_ProjectionData.Orthographic.Size * m_AspectRatio * 0.5f;
			float orthoRight = m_ProjectionData.Orthographic.Size * m_AspectRatio * 0.5f;
			float orthoBottom = -m_ProjectionData.Orthographic.Size * 0.5f;
			float orthoTop = m_ProjectionData.Orthographic.Size * 0.5f;
			float orthoNear = m_ProjectionData.Orthographic.Near;
			float orthoFar = m_ProjectionData.Orthographic.Far;
			m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);
			break;
		}
		}
	}
}