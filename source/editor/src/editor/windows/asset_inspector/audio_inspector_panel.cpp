#include "audio_inspector_panel.h"
#include "engine/audio/audio.h"
#include "editor/windows/utility/property.h"
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
			engine::audio::Stop(m_instance);

			m_audioClip = handle;
			m_instance = {};
		}


		if(engine::audio::IsPlaying(m_instance))
		{
			if (ImGui::Button("Stop"))
			{
				engine::audio::Stop(m_instance);
			}
		}
		else
		{
			if (ImGui::Button("Play"))
			{
				m_instance = engine::audio::Play2D(m_audioClip);
			}
		}

		//const engine::audioClip& clip = engine::assetManager::Getasset<engine::audioClip>(m_audioClip);

		//static struct audioInspectorState
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