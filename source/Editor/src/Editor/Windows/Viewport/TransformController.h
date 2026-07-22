#pragma once

namespace Engine { class Scene; }

namespace Editor
{
	class TransformController
	{
	public:
		void Initialize();
		void DrawGizmo(Engine::Scene& scene, float x, float y, float width, float height);
		bool IsGizmoOver() const;
	};
}