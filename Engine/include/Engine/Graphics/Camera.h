#pragma once

#include "Engine/Core/API.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	class Skybox;

	struct PerspectiveParams 
	{
		float fov = glm::radians(45.0f);
		float near = 0.1f;
		float far = 1000.0f;
	};

	struct OrthographicParams 
	{
		float size = 10.0f;
		float near = -100.0f;
		float far = 100.0f;
	};

	class ENGINE_API Camera
	{
	public:
		enum class Projection { Perspective, Orthographic };
		enum class Background { Color, Skybox };

	public:
		Camera();
		~Camera();

		PerspectiveParams& Perspective() 
		{
			m_DirtyProjection = true;
			return m_Perspective;
		}

		OrthographicParams& Orthographic()
		{
			m_DirtyProjection = true;
			return m_Orthographic;
		}

		void Commit() 
		{
			if (m_DirtyProjection) 
			{
				UpdateProjectionMatrix();
				m_DirtyProjection = false;
			}
		}

		Projection GetProjection() const { return m_Projection; }
		void SetProjection(Projection projection) { m_Projection = projection; UpdateProjectionMatrix(); }

		void SetAspectRatio(float ratio);
		float GetAspectRatio() { return m_AspectRatio; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		const glm::quat GetOrientation() const;
		const glm::vec3 GetForward() const;
		const glm::vec3 GetRight() const;
		const glm::vec3 GetUp() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
		float GetRoll() const { return m_Roll; }

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position);
		void SetRotation(float pitch, float yaw, float roll = 0.0f);
		void SetRotation(const glm::vec3& rotation);

		Background GetBackground() const { return m_Background; }
		void SetBackground(Background background) { m_Background = background; }
		const glm::vec4& GetClearColor() const { return m_ClearColor; }
		void SetClearColor(const glm::vec4& color) { m_ClearColor = color; }

		Skybox& GetSkybox();

	private:
		void UpdateProjectionMatrix();
		void UpdateViewMatrix();

	private:
		Projection m_Projection = Projection::Perspective;
		float m_AspectRatio = 16.0f / 9.0f;
		PerspectiveParams m_Perspective;
		OrthographicParams m_Orthographic;


		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };

		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;
		float m_Roll = 0.0f;


		glm::vec4 m_ClearColor = { 1.0f, 0.05f, 1.0f, 1.0f };
		Skybox* m_Skybox = nullptr;

		Background m_Background = Background::Color;
		bool m_DirtyProjection = true;
	};
}