#include "AudioInspectorPanel.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"
#include "Editor/Windows/Utility/Property.h"
#include <imgui.h>

namespace Editor
{
	void AudioInspectorPanel::Draw(Engine::AudioClip* clip)
	{
		if (!clip)
		{
			ImGui::Text("No audio clip selected");
			return;
		}

		ImGui::Text("Commented out :P");

		//if(Engine::Audio::IsPlaying(clip))
		//{
		//	if (ImGui::Button("Stop"))
		//	{
		//		Engine::Audio::Stop(clip);
		//	}
		//}
		//else
		//{
		//	if (ImGui::Button("Play"))
		//	{
		//		Engine::Audio::Play2D(clip);
		//	}
		//}


		//static struct AudioInspectorState
		//{
		//	float time = 0.0f;
		//	bool  isEditing = false;
		//} state;

		////float duration = clip->GetDurationSeconds();

		//if (!state.isEditing)
		//{
		//	state.time = clip->GetCurrentTimeSeconds();
		//}

		//auto result = PropertyField<float>("Current Time", state.time);

		//if (result.started)
		//{
		//	state.isEditing = true;
		//}

		//if (result.finished)
		//{
		//	clip->SetCurrentTimeSeconds(state.time);
		//	state.isEditing = false;
		//}
	}
}