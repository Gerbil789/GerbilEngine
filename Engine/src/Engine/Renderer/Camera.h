#pragma once

#include <glm/glm.hpp>

namespace Engine 
{
	class Camera 
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection) : m_Projection(projection) {}
		virtual ~Camera() = default;

		//operator glm::mat4& () { return m_Projection; }
		//operator const glm::mat4& () const { return m_Projection; }

		glm::mat4 GetProjection() const { return m_Projection; }

	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}