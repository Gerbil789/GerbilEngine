#include "enginepch.h"
#include "Camera.h"

namespace Engine
{
	Camera::Camera()
	{
		UpdateViewMatrix();
	}

	void Camera::SetViewportSize(const glm::vec2& size)
	{
		m_AspectRatio = size.x / size.y;
		UpdateProjectionMatrix();
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		UpdateViewMatrix();
	}

	void Camera::SetRotation(float pitch, float yaw)
	{
		m_Pitch = pitch;
		m_Yaw = yaw;
		UpdateViewMatrix();
	}

	void Camera::UpdateViewMatrix()
	{
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + GetForward(), GetUp());
	}

	void Camera::UpdateProjectionMatrix()
	{
		switch (m_Type)
		{
		case ProjectionType::Perspective:
		{
			m_ProjectionMatrix = glm::perspectiveRH_ZO(m_Perspective_FOV, m_AspectRatio, m_Perspective_Near, m_Perspective_Far); //RH_ZO - right-handed, zero to one. WebGPU uses this convention.
			break;
		}
		case ProjectionType::Orthographic:
		{
			float halfWidth = m_Orthographic_Size * m_AspectRatio * 0.5f;
			float halfHeight = m_Orthographic_Size * 0.5f;
			m_ProjectionMatrix = glm::orthoRH_ZO(-halfWidth, halfWidth, -halfHeight, halfHeight, m_Orthographic_Near, m_Orthographic_Far);
			break;
		}
		}
	}
}