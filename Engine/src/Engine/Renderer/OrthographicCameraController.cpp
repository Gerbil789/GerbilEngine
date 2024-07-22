#include "enginepch.h"
#include "Engine/Renderer/OrthographicCameraController.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine
{

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation, bool zoom)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation), m_Zoom(zoom) {}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		float rotationRad = glm::radians(m_CameraRotation);
		glm::vec2 forwardDirection = { glm::cos(rotationRad), glm::sin(rotationRad) };
		glm::vec2 rightDirection = { -forwardDirection.y, forwardDirection.x };

		if (Input::IsKeyPressed(Key::A))
			m_CameraPosition -= forwardDirection * m_CameraTranslationSpeed * ts.GetSeconds();
		else if (Input::IsKeyPressed(Key::D))
			m_CameraPosition += forwardDirection * m_CameraTranslationSpeed * ts.GetSeconds();

		if (Input::IsKeyPressed(Key::W))
			m_CameraPosition += rightDirection * m_CameraTranslationSpeed * ts.GetSeconds();
		else if (Input::IsKeyPressed(Key::S))
			m_CameraPosition -= rightDirection * m_CameraTranslationSpeed * ts.GetSeconds();

		if (m_Rotation)
		{
			if (Input::IsKeyPressed(Key::Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			if (Input::IsKeyPressed(Key::E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(glm::vec3(m_CameraPosition.x, m_CameraPosition.y, 0.0f));

		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ENGINE_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(ENGINE_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
		dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(OrthographicCameraController::OnKeyPressed));
	}


	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}


	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		if (m_Zoom) {
			m_ZoomLevel -= e.GetYOffset() * 0.25f;
			m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
			m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		}

	
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}
	bool OrthographicCameraController::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == Key::Space)
		{
			m_CameraPosition = { 0.0f, 0.0f };
			m_CameraRotation = 0.0f;
			m_Camera.SetPosition(glm::vec3(m_CameraPosition.x, m_CameraPosition.y, 0.0f));
			m_Camera.SetRotation(m_CameraRotation);
		}
		return false;
	}
}