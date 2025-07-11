#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/MouseEvent.h"

namespace Editor
{
	class EditorCameraController
	{
	public:
		enum class ControlMode { Orbit, FreeFly };

	public:
		EditorCameraController() = default;

		void SetViewportSize(glm::vec2 size);
		void OnUpdate(Engine::Timestep ts);
		void OnEvent(Engine::Event& e);

		Engine::Camera& GetCamera() { return m_Camera; }

	private:
		void UpdateOrbit(Engine::Timestep ts);
		void UpdateFreeFly(Engine::Timestep ts);

		bool OnMouseScroll(Engine::MouseScrolledEvent& e);
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

	private:
		ControlMode m_Mode = ControlMode::Orbit;
		Engine::Camera m_Camera;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_LastMousePosition = { 0.0f, 0.0f };

		glm::vec3 m_FocalPoint = glm::vec3(0.0f);
		float m_Distance = 10.0f;

		float m_MoveSpeed = 5.0f;
		float m_RotationSpeed = 0.003f;
		std::pair<float, float> PanSpeed() const;
		float ZoomSpeed() const;

	};
}