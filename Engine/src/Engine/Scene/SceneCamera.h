#pragma once

#include "Engine/Renderer/Camera.h"

namespace Engine
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
	private:
		void RecalculateProjection();

		float m_AspectRatio = 16.0f / 9.0f;
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar = 1.0f;
		//float m_PerspectiveFOV = glm::radians(45.0f);
		//float m_PerspectiveNear = 0.01f;
		//float m_PerspectiveFar = 1000.0f;
		bool m_IsOrthographic = true;
	};
}