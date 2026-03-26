#include "enginepch.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/Skybox.h"
#include <glm/gtx/quaternion.hpp>

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

	static glm::mat4 LookAtLH(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
	{
		glm::vec3 f = glm::normalize(target - eye);
		glm::vec3 r = glm::normalize(glm::cross(up, f));
		glm::vec3 u = glm::cross(f, r);

		glm::mat4 result(1.0f);

		result[0][0] = r.x;
		result[1][0] = r.y;
		result[2][0] = r.z;
		result[3][0] = -glm::dot(r, eye);

		result[0][1] = u.x;
		result[1][1] = u.y;
		result[2][1] = u.z;
		result[3][1] = -glm::dot(u, eye);

		result[0][2] = f.x;
		result[1][2] = f.y;
		result[2][2] = f.z;
		result[3][2] = -glm::dot(f, eye);

		return result;
	}

	static glm::mat4 PerspectiveLH_ZO(float fov, float aspect, float nearZ, float farZ)
	{
		float yScale = 1.0f / tanf(fov * 0.5f);
		float xScale = yScale / aspect;

		glm::mat4 result(0.0f);

		result[0][0] = xScale;
		result[1][1] = yScale;
		result[2][2] = farZ / (farZ - nearZ);
		result[2][3] = 1.0f;
		result[3][2] = -(nearZ * farZ) / (farZ - nearZ);

		return result;
	}

	static glm::mat4 OrthoLH_ZO(float left, float right, float bottom, float top, float nearZ, float farZ)
	{
		glm::mat4 result(1.0f);

		result[0][0] = 2.0f / (right - left);
		result[1][1] = 2.0f / (top - bottom);
		result[2][2] = 1.0f / (farZ - nearZ);

		result[3][0] = -(right + left) / (right - left);
		result[3][1] = -(top + bottom) / (top - bottom);
		result[3][2] = -nearZ / (farZ - nearZ);

		return result;
	}


	void Camera::UpdateViewMatrix()
	{
		//m_ViewMatrix = glm::lookAtLH(m_Position, m_Position + GetForward(), GetUp());
		m_ViewMatrix = LookAtLH(m_Position, m_Position + GetForward(), GetUp());
	}

	void Camera::UpdateProjectionMatrix()
	{
		switch (m_Projection)
		{
		case Projection::Perspective:
		{
			//m_ProjectionMatrix = glm::perspectiveLH_ZO(m_Perspective.fov, m_AspectRatio, m_Perspective.near, m_Perspective.far); //LH_ZO - left-handed, zero to one. WebGPU uses this convention.
			m_ProjectionMatrix = PerspectiveLH_ZO(m_Perspective.fov, m_AspectRatio, m_Perspective.near, m_Perspective.far);
			break;
		}
		case Projection::Orthographic:
		{
			float halfHeight = m_Orthographic.size * 0.5f;
			float halfWidth = halfHeight * m_AspectRatio;
			//m_ProjectionMatrix = glm::orthoLH_ZO(-halfWidth, halfWidth, -halfHeight, halfHeight, m_Orthographic.near, m_Orthographic.far);
			m_ProjectionMatrix = OrthoLH_ZO(-halfWidth, halfWidth, -halfHeight, halfHeight, m_Orthographic.near, m_Orthographic.far);
			break;
		}
		}
	}

	Skybox& Camera::GetSkybox()
	{
		return *m_Skybox;
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