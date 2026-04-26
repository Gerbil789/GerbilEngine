#pragma once

#include "Engine/Core/API.h"
#include <glm/glm.hpp>

namespace Engine
{
	class ENGINE_API Camera
	{
	public:
		enum class Projection { Perspective, Orthographic };
		enum class Background { Color, Skybox };

	public:
		Projection GetProjection() const;
		void SetProjection(Projection projection);

		void SetAspectRatio(float ratio);
		float GetAspectRatio() const { return m_AspectRatio; }

		const glm::mat4& GetProjectionMatrix() const;
		const glm::mat4& GetViewMatrix() const;

		const glm::quat GetOrientation() const;
		const glm::vec3 GetForward() const;
		const glm::vec3 GetRight() const;
		const glm::vec3 GetUp() const;

		float GetPitch() const;
		float GetYaw() const;
		float GetRoll() const;

		const glm::vec3& GetPosition() const;
		void SetPosition(const glm::vec3& position);
		void SetRotation(float pitch, float yaw, float roll = 0.0f);
		void SetRotation(const glm::vec3& rotation);

		Background GetBackground() const;
		void SetBackground(Background background);

		const glm::vec4& GetClearColor() const;
		void SetClearColor(const glm::vec4& color);

		float GetPerspectiveFOV() const;
		float GetPerspectiveNear() const;
		float GetPerspectiveFar() const;
		void SetPerspectiveFOV(float fov);
		void SetPerspectiveNear(float near);
		void SetPerspectiveFar(float far);

		float GetOrthoSize() const;
		float GetOrthoNear() const;
		float GetOrthoFar() const;
		void SetOrthoSize(float size);
		void SetOrthoNear(float near);
		void SetOrthoFar(float far);

		std::array<glm::vec3, 8> GetFrustumCornersWorld(float near, float far) const;

	private:
		void UpdateProjectionMatrix();
		void UpdateViewMatrix();

	private:
		Projection m_Projection = Projection::Perspective;
		float m_AspectRatio = { 16.0f / 9.0f };

		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };

		glm::vec4 m_ClearColor = { 1.0f, 0.05f, 1.0f, 1.0f };
		Background m_Background = Background::Color;

		struct Perspective
		{
			float fov = glm::radians(45.0f);
			float nearClip = 0.1f;
			float farClip = 512.0f;
		} m_Perspective;

		struct Orthographic
		{
			float size = 10.0f;
			float nearClip = -1.0f;
			float farClip = 512.0f;
		} m_Orthographic;
	};
}