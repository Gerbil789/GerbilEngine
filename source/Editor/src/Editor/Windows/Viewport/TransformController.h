#pragma once

namespace Engine { class SceneAsset; }

namespace Editor
{
	class TransformController
	{
	public:
		void Initialize();
		void DrawGizmo(Engine::SceneAsset& scene, float x, float y, float width, float height);
		bool IsGizmoOver() const;
	};
}