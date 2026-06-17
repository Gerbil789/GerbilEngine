#include "AudioInspectorPanel.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Asset/AssetManager.h"
#include <imgui.h>

namespace Editor
{
	namespace
	{
		Engine::Uuid m_AudioClip;
		Engine::Uuid m_Instance;
	}

	void AudioInspectorPanel::Draw(Engine::Uuid id)
	{
		if(id != m_AudioClip)
		{
			Engine::Audio::Stop(m_Instance);

			m_AudioClip = id;
			m_Instance = Engine::Uuid{};
		}


		if(Engine::Audio::IsPlaying(m_Instance))
		{
			if (ImGui::Button("Stop"))
			{
				Engine::Audio::Stop(m_Instance);
			}
		}
		else
		{
			if (ImGui::Button("Play"))
			{
				m_Instance = Engine::Audio::Play2D(m_AudioClip);
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