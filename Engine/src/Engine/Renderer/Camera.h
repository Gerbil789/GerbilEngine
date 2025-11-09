#pragma once

#include "Engine/Core/Core.h"
//#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Skybox.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	class Camera
	{
	public:
		enum class ProjectionType { Perspective, Orthographic };
		enum class BackgroundType { Color, Skybox };

	public:
		Camera();

		ProjectionType GetType() const { return m_Type; }
		void SetType(ProjectionType type) { m_Type = type; UpdateProjectionMatrix(); }

		void SetViewportSize(const glm::vec2& size);

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

		void SetBackgroundType(BackgroundType type) { m_BackgroundType = type; }
		const BackgroundType GetBackgroundType() const { return m_BackgroundType; }
		glm::vec4 GetClearColor() const { return m_ClearColor; }
		void SetClearColor(const glm::vec4& color) { m_ClearColor = color; }
		//void SetSkyboxTexture(const Ref<CubeMapTexture>& texture) { m_Skybox = skybox; }

		Skybox& GetSkybox() { return m_Skybox; }

	private:
		void UpdateProjectionMatrix();
		void UpdateViewMatrix();

	private:
		ProjectionType m_Type = ProjectionType::Perspective;
		float m_AspectRatio = 16.0f / 9.0f;
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_Position = { 0.0f, 0.0f, 10.0f };
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;

		float m_Perspective_FOV = glm::radians(45.0f);
		float m_Perspective_Near = 0.1f;
		float m_Perspective_Far = 1000.0f;

		float m_Orthographic_Size = 10.0f;
		float m_Orthographic_Near = -100.0f;
		float m_Orthographic_Far = 100.0f;

		BackgroundType m_BackgroundType = BackgroundType::Color;
		glm::vec4 m_ClearColor = { 0.05f, 0.05f, 0.05f, 1.0f };
		Skybox m_Skybox;
	};
}