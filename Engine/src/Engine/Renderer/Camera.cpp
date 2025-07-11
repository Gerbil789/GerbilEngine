#include "enginepch.h"
#include "Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	void Camera::SetViewportSize(glm::vec2 size)
	{
		m_AspectRatio = size.x / size.y;
		RecalculateProjection();
	}

	void Camera::SetProjectionData(const ProjectionData& data)
	{
		if (data != m_ProjectionData)
		{
			m_ProjectionData = data;
			RecalculateProjection();
		}
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		RecalculateView();
	}

	void Camera::SetRotation(float pitch, float yaw)
	{
		m_Pitch = pitch;
		m_Yaw = yaw;
		RecalculateView();
	}

	const glm::vec3 Camera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	const glm::vec3 Camera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	const glm::vec3 Camera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	const glm::quat Camera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}


	void Camera::RecalculateView()
	{
		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void Camera::RecalculateProjection()
	{
		switch (m_ProjectionData.Type)
		{
		case ProjectionType::Perspective:
			m_ProjectionMatrix = glm::perspective(m_ProjectionData.Perspective.FOV, m_AspectRatio, m_ProjectionData.Perspective.Near, m_ProjectionData.Perspective.Far);
			break;

		case ProjectionType::Orthographic:
		{
			float orthoLeft = -m_ProjectionData.Orthographic.Size * m_AspectRatio * 0.5f;
			float orthoRight = m_ProjectionData.Orthographic.Size * m_AspectRatio * 0.5f;
			float orthoBottom = -m_ProjectionData.Orthographic.Size * 0.5f;
			float orthoTop = m_ProjectionData.Orthographic.Size * 0.5f;

			m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_ProjectionData.Orthographic.Near, m_ProjectionData.Orthographic.Far);
			break;
		}

		default:
			ASSERT(false, "Unknown projection type!");
		}
	}
}