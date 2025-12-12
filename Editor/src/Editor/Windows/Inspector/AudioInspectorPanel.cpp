#include "enginepch.h"
#include "AudioInspectorPanel.h"
#include "Engine/Audio/Audio.h"

#include <imgui.h>

namespace Editor
{
	static ma_sound* s_CurrentlyPlayingSound = nullptr;

	void AudioInspectorPanel::Draw(Ref<Engine::AudioClip> audioClip)
	{
		if (!audioClip)
		{
			ImGui::Text("No audio clip selected");
			return;
		}

		int resolution = audioClip->GetWaveformResolution();
		const float* minArr = audioClip->GetWaveformMin();
		const float* maxArr = audioClip->GetWaveformMax();

		ImDrawList* dl = ImGui::GetWindowDrawList();
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		float width = ImGui::GetContentRegionAvail().x;
		float height = 150.0f;

		for (int i = 0; i < resolution; i++)
		{
			float t = (float)i / (float)resolution;
			float x = p0.x + t * width;

			float ymin = (1.0f - ((minArr[i] + 1.0f) * 0.5f)) * height;
			float ymax = (1.0f - ((maxArr[i] + 1.0f) * 0.5f)) * height;

			dl->AddLine(
				ImVec2(x, p0.y + ymin),
				ImVec2(x, p0.y + ymax),
				IM_COL32(255, 255, 255, 255)
			);
		}

		ImGui::Dummy(ImVec2(width, height)); // Reserve space

		if (s_CurrentlyPlayingSound)
		{
			ma_uint64 cursorFrames = 0;
			ma_sound_get_cursor_in_pcm_frames(s_CurrentlyPlayingSound, &cursorFrames);

			float t = (float)cursorFrames / (float)audioClip->GetTotalFrames();
			float x = p0.x + t * width;

			dl->AddLine(
				ImVec2(x, p0.y),
				ImVec2(x, p0.y + height),
				IM_COL32(255, 0, 0, 255),
				2.0f
			);
		}

		ImGui::Separator();

		if(ImGui::Button("Play"))
		{
			s_CurrentlyPlayingSound = Engine::Audio::Play(audioClip);
		}

		ImGui::SameLine();

		if (ImGui::Button("Stop")) 
		{
			Engine::Audio::Stop(s_CurrentlyPlayingSound);
		}



	}
}