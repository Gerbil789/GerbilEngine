#pragma once

#include <glm/glm.hpp>

namespace editor
{
	struct EditorSettings
	{
		bool showGrid = true;
		glm::vec4 wireframeColor = glm::vec4{ 1.0f, 0.0f, 1.0f, 1.0f };
	};
};