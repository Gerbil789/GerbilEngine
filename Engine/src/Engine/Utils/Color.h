#pragma once
#include <glm/glm.hpp>

namespace Engine 
{
	class Color 
	{
	public:
		static glm::vec4 Radnom() { return { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.0f }; }

		static glm::vec4 White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
		static glm::vec4 Black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
		static glm::vec4 Red() { return { 0.8f, 0.3f, 0.2f, 1.0f }; }
		static glm::vec4 Green() { return { 0.2f, 0.8f, 0.2f, 1.0f }; }
		static glm::vec4 Blue() { return { 0.2f, 0.3f, 0.8f, 1.0f }; }
		static glm::vec4 Yellow() { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
		static glm::vec4 Cyan() { return { 0.0f, 1.0f, 1.0f, 1.0f }; }
		static glm::vec4 Magenta() { return { 1.0f, 0.0f, 1.0f, 1.0f }; }
		static glm::vec4 Gray() { return { 0.5f, 0.5f, 0.5f, 1.0f }; }
		static glm::vec4 LightGray() { return { 0.75f, 0.75f, 0.75f, 1.0f }; }
		static glm::vec4 DarkGray() { return { 0.25f, 0.25f, 0.25f, 1.0f }; }
		static glm::vec4 Transparent() { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
	};
}