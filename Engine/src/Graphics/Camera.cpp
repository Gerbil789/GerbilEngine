#include "enginepch.h"
#include "Engine/Graphics/Camera.h"
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	Camera::Projection Camera::GetProjection() const
	{
		return m_Projection;
	}

	void Camera::SetProjection(Projection projection)
	{
		m_Projection = projection; 
		UpdateProjectionMatrix();
	}

	void Camera::SetAspectRatio(float ratio)
	{
		m_AspectRatio = ratio;
		UpdateProjectionMatrix();
	}

	const glm::mat4& Camera::GetProjectionMatrix() const
	{
		return m_ProjectionMatrix;
	}

	const glm::mat4& Camera::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	const glm::quat Camera::GetOrientation() const
	{
		return glm::quat(glm::radians(m_Rotation));
	}

	const glm::vec3 Camera::GetForward() const
	{
		return GetOrientation() * glm::vec3(0.0f, 0.0f, 1.0f);
	}

	const glm::vec3 Camera::GetRight() const
	{
		return GetOrientation() * glm::vec3(1.0f, 0.0f, 0.0f);
	}

	const glm::vec3 Camera::GetUp() const
	{
		return GetOrientation() * glm::vec3(0.0f, 1.0f, 0.0f);
	}

	float Camera::GetPitch() const
	{
		return m_Rotation.x;
}

	float Camera::GetYaw() const
	{
		return m_Rotation.y;
	}

	float Camera::GetRoll() const
	{
		return m_Rotation.z;
	}

	const glm::vec3& Camera::GetPosition() const
	{
		return m_Position;
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		UpdateViewMatrix();
	}

	void Camera::SetRotation(float pitch, float yaw, float roll)
	{
		m_Rotation = { pitch, yaw, roll };
		UpdateViewMatrix();
	}

	void Camera::SetRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;
		UpdateViewMatrix();
	}

	Camera::Background Camera::GetBackground() const
	{
		return m_Background;
	}

	void Camera::SetBackground(Background background)
	{
		m_Background = background;
	}

	glm::vec4& Camera::GetClearColor()
	{ 
		return m_ClearColor; 
	}

	void Camera::SetClearColor(const glm::vec4& color)
	{
		m_ClearColor = color;
	}

	float Camera::GetPerspectiveFOV() const
	{
		return m_Perspective.fov;
	}

	float Camera::GetPerspectiveNear() const
	{
		return m_Perspective.nearClip;
	}

	float Camera::GetPerspectiveFar() const
	{
		return m_Perspective.farClip;
	}

	void Camera::SetPerspectiveFOV(float fov)
	{
		m_Perspective.fov = fov;
		UpdateProjectionMatrix();
	}

	void Camera::SetPerspectiveNear(float near)
	{
		m_Perspective.nearClip = near;
		UpdateProjectionMatrix();
	}

	void Camera::SetPerspectiveFar(float far)
	{
		m_Perspective.farClip = far;
		UpdateProjectionMatrix();
	}

	float Camera::GetOrthoSize() const
	{
		return m_Orthographic.size;
	}

	float Camera::GetOrthoNear() const
	{
		return m_Orthographic.nearClip;
	}

	float Camera::GetOrthoFar() const
	{
		return m_Orthographic.farClip;
	}

	void Camera::SetOrthoSize(float size)
	{
		m_Orthographic.size = size;
		UpdateProjectionMatrix();
	}

	void Camera::SetOrthoNear(float near)
	{
		m_Orthographic.nearClip = near;
		UpdateProjectionMatrix();
	}

	void Camera::SetOrthoFar(float far)
	{
		m_Orthographic.farClip = far;
		UpdateProjectionMatrix();
	}

	
	void Camera::UpdateViewMatrix()
	{
		m_ViewMatrix = glm::lookAtLH(m_Position, m_Position + GetForward(), GetUp());
	}

	void Camera::UpdateProjectionMatrix()
	{
		switch (m_Projection)
		{
		case Projection::Perspective:
		{
			m_ProjectionMatrix = glm::perspectiveLH_ZO(m_Perspective.fov, m_AspectRatio, m_Perspective.nearClip, m_Perspective.farClip); //LH_ZO - left-handed, zero to one. WebGPU uses this convention.
			break;
		}
		case Projection::Orthographic:
		{
			float halfHeight = m_Orthographic.size * 0.5f;
			float halfWidth = halfHeight * m_AspectRatio;
			m_ProjectionMatrix = glm::orthoLH_ZO(-halfWidth, halfWidth, -halfHeight, halfHeight, m_Orthographic.nearClip, m_Orthographic.farClip);
			break;
		}
		}
	}

	std::array<glm::vec3, 8> Camera::GetFrustumCornersWorld(float near, float far) const
	{
		glm::mat4 invView = glm::inverse(GetViewMatrix());

		std::array<glm::vec3, 8> corners;

		corners[0] = { glm::tan(m_Perspective.fov * 0.5f) * near * m_AspectRatio, glm::tan(m_Perspective.fov * 0.5f) * near, near };
		corners[1] = { -corners[0].x, corners[0].y, corners[0].z };
		corners[2] = { -corners[0].x, -corners[0].y, corners[0].z };
		corners[3] = { corners[0].x, -corners[0].y, corners[0].z };

		corners[4] = { glm::tan(m_Perspective.fov * 0.5f) * far * m_AspectRatio, glm::tan(m_Perspective.fov * 0.5f) * far, far };
		corners[5] = { -corners[4].x, corners[4].y, corners[4].z };
		corners[6] = { -corners[4].x, -corners[4].y, corners[4].z };
		corners[7] = { corners[4].x, -corners[4].y, corners[4].z };

		for (auto& corner : corners)
		{
			corner = glm::vec3(invView * glm::vec4(corner, 1.0f));
		}

		return corners;
	}
}