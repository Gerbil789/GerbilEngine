#pragma once
#include <glm/glm.hpp>

namespace Engine
{
	class Color
	{
	public:
		static const glm::vec4 Random() { return { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.0f }; }

		static const glm::vec4 White;
		static const glm::vec4 Black;
		static const glm::vec4 Red;
		static const glm::vec4 Green;
		static const glm::vec4 Blue;
		static const glm::vec4 Yellow;
		static const glm::vec4 Cyan;
		static const glm::vec4 Magenta;
		static const glm::vec4 Gray;
		static const glm::vec4 LightGray;
		static const glm::vec4 DarkGray;
		static const glm::vec4 Brown;
		static const glm::vec4 Orange;
		static const glm::vec4 Pink;
		static const glm::vec4 Purple;

		static const glm::vec4 SemiTransparent;
		static const glm::vec4 Transparent;
	};
}
