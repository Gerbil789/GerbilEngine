#pragma once

namespace Engine
{
	struct ViewportState
	{
		float positionX = 0.0f;
		float positionY = 0.0f;

		float width = 0.0f;
		float height = 0.0f;
	};

	ViewportState viewportState;
}