#pragma once

namespace Engine { class AudioClip; }

namespace Editor
{
	class AudioInspectorPanel
	{
	public:
		void Draw(Engine::AudioClip* clip);
	};
}