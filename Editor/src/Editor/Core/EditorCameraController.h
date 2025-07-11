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
		EditorCameraController(Engine::Camera* camera) : m_Camera(camera) {}
		void OnUpdate(Engine::Timestep ts);
		void OnEvent(Engine::Event& e);

	private:
		bool OnMouseScroll(Engine::MouseScrolledEvent& e);
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

	private:
		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;


		Engine::Camera* m_Camera = nullptr;
		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		glm::vec3 m_FocalPoint;
	};
}