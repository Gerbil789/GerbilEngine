#pragma once

#include "Engine/Core/API.h"

namespace Engine
{
	struct ViewportState
	{
		float positionX = 0.0f;
		float positionY = 0.0f;

		float width = 0.0f;
		float height = 0.0f;
	};

	extern ENGINE_API ViewportState viewportState;
}