#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	class Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

		struct ProjectionData 
		{
			ProjectionType Type = ProjectionType::Perspective;

			struct Perspective
			{
				float FOV = glm::radians(45.0f);
				float Near = 0.1f;
				float Far = 1000.0f;

				bool operator==(const Perspective& other) const = default;
			} Perspective;

			struct Orthographic
			{
				float Size = 10.0f;
				float Near = -100.0f;
				float Far = 100.0f;

				bool operator==(const Orthographic& other) const = default;
			} Orthographic;

			bool operator==(const ProjectionData& other) const = default;
		};

	public:
		Camera();

		void SetViewportSize(const glm::vec2& size);

		const ProjectionData& GetProjectionData() const { return m_ProjectionData; } //TODO: remove? make serializer a friend class instead?

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		const glm::quat GetOrientation() const { return glm::quat(glm::radians(glm::vec3(m_Pitch, m_Yaw, 0.0f))); }
		const glm::vec3 GetForward() const { return GetOrientation() * glm::vec3(0.0f, 0.0f, -1.0f); }
		const glm::vec3 GetRight() const { return GetOrientation() * glm::vec3(1.0f, 0.0f, 0.0f); }
		const glm::vec3 GetUp() const { return GetOrientation() * glm::vec3(0.0f, 1.0f, 0.0f); }

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position);
		void SetRotation(float pitch, float yaw);

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

	private:
		void UpdateProjectionMatrix();
		void UpdateViewMatrix();

	private:
		ProjectionData m_ProjectionData;
		float m_AspectRatio = 16.0f / 9.0f;
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_Position = { 0.0f, 0.0f, 10.0f };
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;
	};
}