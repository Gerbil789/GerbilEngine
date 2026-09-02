#pragma once

namespace engine { class SceneAsset; }

namespace editor
{
	class TransformController
	{
	public:
		void Initialize();
		void DrawGizmo(engine::SceneAsset& scene, float x, float y, float width, float height);
		bool IsGizmoOver() const;
	};
}