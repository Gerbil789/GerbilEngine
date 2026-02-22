#include "enginepch.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/Skybox.h"

namespace Engine
{
	Camera::Camera()
	{
		m_Skybox = new Skybox();
		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}

	Camera::~Camera()
	{
		delete m_Skybox;
	}

	void Camera::SetAspectRatio(float ratio)
	{
		m_AspectRatio = ratio;
		UpdateProjectionMatrix();
	}

	const glm::quat Camera::GetOrientation() const
	{
		return glm::quat(glm::radians(glm::vec3(m_Pitch, m_Yaw, m_Roll)));
	}

	const glm::vec3 Camera::GetForward() const
	{
		return GetOrientation() * glm::vec3(0.0f, 0.0f, -1.0f);
	}

	const glm::vec3 Camera::GetRight() const
	{
		return GetOrientation() * glm::vec3(1.0f, 0.0f, 0.0f);
	}

	const glm::vec3 Camera::GetUp() const
	{
		return GetOrientation() * glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		UpdateViewMatrix();
	}

	void Camera::SetRotation(float pitch, float yaw, float roll)
	{
		m_Pitch = pitch;
		m_Yaw = yaw;
		m_Roll = roll;
		UpdateViewMatrix();
	}

	void Camera::SetRotation(const glm::vec3& rotation)
	{
		m_Pitch = rotation.x;
		m_Yaw = rotation.y;
		m_Roll = rotation.z;
		UpdateViewMatrix();
	}

	void Camera::UpdateViewMatrix()
	{
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + GetForward(), GetUp());
	}

	void Camera::UpdateProjectionMatrix()
	{
		switch (m_Projection)
		{
		case Projection::Perspective:
		{
			m_ProjectionMatrix = glm::perspectiveRH_ZO(m_Perspective.fov, m_AspectRatio, m_Perspective.near, m_Perspective.far); //RH_ZO - right-handed, zero to one. WebGPU uses this convention.
			break;
		}
		case Projection::Orthographic:
		{
			float halfHeight = m_Orthographic.size * 0.5f;
			float halfWidth = halfHeight * m_AspectRatio;
			m_ProjectionMatrix = glm::orthoRH_ZO(-halfWidth, halfWidth, -halfHeight, halfHeight, m_Orthographic.near, m_Orthographic.far);
			break;
		}
		}
	}

	Skybox& Camera::GetSkybox()
	{
		return *m_Skybox;
	}
}