#pragma once

namespace editor
{
	class TransformController
	{
	public:
		void Initialize();
		void DrawGizmo(float x, float y, float width, float height);
		bool IsGizmoOver() const;
	};
}