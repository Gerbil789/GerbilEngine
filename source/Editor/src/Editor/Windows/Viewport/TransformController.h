#pragma once

#include "Engine/Core/Scene.h"

namespace editor
{
	class TransformController
	{
	public:
		void Initialize();
		void DrawGizmo(engine::Scene scene, float x, float y, float width, float height);
		bool IsGizmoOver() const;
	};
}