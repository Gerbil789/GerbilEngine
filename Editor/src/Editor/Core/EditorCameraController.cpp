#include "EditorCameraController.h"
#include "Engine/Core/Input.h"

namespace Editor
{
	using namespace Engine;

	void EditorCameraController::SetViewportSize(glm::vec2 size)
	{
		m_ViewportSize = size;
		m_Camera.SetViewportSize(size);
	}

	void EditorCameraController::OnUpdate(Timestep ts)
	{
		m_Mode = Input::IsKeyPressed(Key::LeftShift) ? ControlMode::FreeFly : ControlMode::Orbit;

		UpdateFreeFly(ts);

		//if(m_Mode == ControlMode::FreeFly)
		//{
		//	UpdateFreeFly(ts);
		//}
		//else
		//{
		//	UpdateOrbit(ts);
		//}
	}

	void EditorCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ENGINE_BIND_EVENT_FN(OnMouseScroll));
	}

	void EditorCameraController::UpdateOrbit(Engine::Timestep ts)
	{
		if (Input::IsKeyPressed(Key::LeftAlt))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = (mouse - m_LastMousePosition) * 0.003f;
			m_LastMousePosition = mouse;

			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseZoom(delta.y);
		}
	}

	void EditorCameraController::UpdateFreeFly(Engine::Timestep ts)
	{
		glm::vec2 mousePos = Input::GetMousePosition();
		glm::vec2 delta = (mousePos - m_LastMousePosition) * m_RotationSpeed;
		m_LastMousePosition = mousePos;

		if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
		{
			float pitch = m_Camera.GetPitch() + delta.y;
			float yaw = m_Camera.GetYaw() + delta.x;
			m_Camera.SetRotation(pitch, yaw);
		}

		glm::vec3 position = m_Camera.GetPosition();
		glm::vec3 forward = m_Camera.GetForwardDirection();
		glm::vec3 right = m_Camera.GetRightDirection();
		glm::vec3 up = m_Camera.GetUpDirection();

		if (Input::IsKeyPressed({ Key::W, Key::Up })) position += forward * m_MoveSpeed * (float)ts;
		if (Input::IsKeyPressed({ Key::S, Key::Down })) position -= forward * m_MoveSpeed * (float)ts;
		if (Input::IsKeyPressed({ Key::A, Key::Left })) position -= right * m_MoveSpeed * (float)ts;
		if (Input::IsKeyPressed({ Key::D, Key::Right })) position += right * m_MoveSpeed * (float)ts;
		if (Input::IsKeyPressed(Key::Q)) position -= up * m_MoveSpeed * (float)ts;
		if (Input::IsKeyPressed(Key::E)) position += up * m_MoveSpeed * (float)ts;

		m_Camera.SetPosition(position);
	}

	bool EditorCameraController::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		//m_Camera->RecalculateView();
		return false;
	}



	void EditorCameraController::MouseZoom(float delta)
	{
		/*m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += m_Camera->GetForwardDirection();
			m_Distance = 1.0f;
		}*/
	}

	float EditorCameraController::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	std::pair<float, float> EditorCameraController::PanSpeed() const
	{
		//float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float x = 1.0f;
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		//float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float y = 1.0f;
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}


	void EditorCameraController::MousePan(const glm::vec2& delta)
	{
		//auto [xSpeed, ySpeed] = PanSpeed();
		//m_FocalPoint += -m_Camera->GetRightDirection() * delta.x * xSpeed * m_Distance;
		//m_FocalPoint += m_Camera->GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCameraController::MouseRotate(const glm::vec2& delta)
	{
		//float yawSign = m_Camera->GetUpDirection().y < 0 ? -1.0f : 1.0f;
		//m_Camera->GetYaw() += yawSign * delta.x * RotationSpeed();
		//m_Camera->GetPitch() += delta.y * RotationSpeed();
	}

}


