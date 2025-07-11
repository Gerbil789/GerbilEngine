#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	class Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

		struct ProjectionData {
			ProjectionType Type = ProjectionType::Perspective;

			struct Perspective
			{
				float FOV = glm::radians(45.0f);
				float Near = 0.01f;
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
		Camera() = default;

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetProjectionData(const ProjectionData& data);
		const ProjectionData& GetProjectionData() const { return m_ProjectionData; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position);

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
		void SetRotation(float pitch, float yaw);

		const glm::vec3 GetUpDirection() const;
		const glm::vec3 GetRightDirection() const;
		const glm::vec3 GetForwardDirection() const;
		const glm::quat GetOrientation() const;

	private:
		void RecalculateProjection();
		void RecalculateView();

	private:
		ProjectionData m_ProjectionData;

		float m_AspectRatio = 16.0f / 9.0f;
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;
	};
}