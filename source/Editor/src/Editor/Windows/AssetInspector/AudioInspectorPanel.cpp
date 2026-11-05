#include "AudioInspectorPanel.h"
#include "Engine/Audio/Audio.h"
#include "Editor/Windows/Utility/Property.h"
#include <imgui.h>

namespace editor
{
	namespace
	{
		engine::AudioClipHandle m_AudioClip;
		engine::AudioInstance m_Instance;
	}

	void AudioInspectorPanel::Draw(engine::AudioClipHandle handle)
	{
		if(handle != m_AudioClip)
		{
			engine::Audio::Stop(m_Instance);

			m_AudioClip = handle;
			m_Instance = {};
		}


		if(engine::Audio::IsPlaying(m_Instance))
		{
			if (ImGui::Button("Stop"))
			{
				engine::Audio::Stop(m_Instance);
			}
		}
		else
		{
			if (ImGui::Button("Play"))
			{
				m_Instance = engine::Audio::Play2D(m_AudioClip);
			}
		}

		//const Engine::AudioClip& clip = Engine::AssetManager::GetAsset<Engine::AudioClip>(m_AudioClip);

		//static struct AudioInspectorState
		//{
		//	float time = 0.0f;
		//	bool  isEditing = false;
		//} state;

		//float duration = clip->GetDurationSeconds();

		//if (!state.isEditing)
		//{
		//	state.time = clip.GetCurrentTimeSeconds();
		//}

		//PropertyTable table;

		//float time = clip.
		//auto result = PropertyField("Current Time", time);

		/*if (result.started)
		{
			state.isEditing = true;
		}

		if (result.finished)
		{
			clip.SetCurrentTimeSeconds(state.time);
			state.isEditing = false;
		}*/
	}
}