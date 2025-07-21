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
		switch (m_ProjectionData.Type)
		{
		case ProjectionType::Perspective:
		{
			float fov = m_ProjectionData.Perspective.FOV;
			float nearZ = m_ProjectionData.Perspective.Near;
			float farZ = m_ProjectionData.Perspective.Far;
			m_ProjectionMatrix = glm::perspectiveRH_ZO(fov, m_AspectRatio, nearZ, farZ); //RH_ZO - right-handed, zero to one. WebGPU uses this convention.
			break;
		}
		case ProjectionType::Orthographic:
		{
			float size = m_ProjectionData.Orthographic.Size;
			float nearZ = m_ProjectionData.Orthographic.Near;
			float farZ = m_ProjectionData.Orthographic.Far;
			float halfWidth = size * m_AspectRatio * 0.5f;
			float halfHeight = size * 0.5f;
			m_ProjectionMatrix = glm::orthoRH_ZO(-halfWidth, halfWidth, -halfHeight, halfHeight, nearZ, farZ);
			break;
		}
		}
	}
}