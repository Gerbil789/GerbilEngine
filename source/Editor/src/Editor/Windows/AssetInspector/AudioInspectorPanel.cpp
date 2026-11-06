#include "AudioInspectorPanel.h"
#include "Engine/Audio/Audio.h"
#include "Editor/Windows/Utility/Property.h"
#include <imgui.h>

namespace editor
{
	namespace
	{
		engine::AudioClipHandle m_audioClip;
		engine::AudioInstance m_instance;
	}

	void AudioInspectorPanel::Draw(engine::AudioClipHandle handle)
	{
		if(handle != m_audioClip)
		{
			engine::Audio::Stop(m_instance);

			m_audioClip = handle;
			m_instance = {};
		}


		if(engine::Audio::IsPlaying(m_instance))
		{
			if (ImGui::Button("Stop"))
			{
				engine::Audio::Stop(m_instance);
			}
		}
		else
		{
			if (ImGui::Button("Play"))
			{
				m_instance = engine::Audio::Play2D(m_audioClip);
			}
		}

		//const Engine::AudioClip& clip = Engine::AssetManager::GetAsset<Engine::AudioClip>(m_audioClip);

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