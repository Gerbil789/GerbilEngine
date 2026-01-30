#pragma once

#include "Engine/Scene/Entity.h"

namespace Editor
{
	class EntityInspectorPanel
	{
	public:
		void Draw(Engine::Entity entity);

	private:
		void DrawTransform();
		void DrawCamera();
		void DrawMesh();
		void DrawAudioListener();
		void DrawAudioSource();
		void DrawScript();
		void DrawAddComponentButton();

	private:
		Engine::Entity m_Entity;
	};
}