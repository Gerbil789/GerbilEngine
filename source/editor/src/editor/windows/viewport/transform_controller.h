#pragma once

namespace editor
{
	struct TransformController
	{
		void Initialize();
		void DrawGizmo(float x, float y, float width, float height);
		bool IsGizmoOver() const;
	};
}