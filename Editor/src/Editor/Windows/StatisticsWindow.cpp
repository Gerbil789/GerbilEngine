#include "enginepch.h"
#include "StatisticsWindow.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Core/Time.h"
#include <imgui.h>

namespace Editor
{
	static constexpr int HISTORY_LENGTH = 600; // 10 seconds @ 60 fps
	static float fpsHistory[HISTORY_LENGTH] = { 0.0f };
	static float msHistory[HISTORY_LENGTH] = { 0.0f };
	static int offset = 0;

	void StatisticsWindow::OnUpdate()
	{
		float fps = Engine::Time::FPS();
		float ms = 1000.0f / fps;

		fpsHistory[offset] = fps;
		msHistory[offset] = ms;
		offset = (offset + 1) % HISTORY_LENGTH;

		ImGui::Begin("Statistics");

		ImVec2 fullWidth = ImVec2(-1, 80);  // Full width, fixed height

		ImGui::Text("FPS: %.0f", fps);
		ImGui::PlotLines("##FPS", fpsHistory, HISTORY_LENGTH, offset, nullptr, 0.0f, 200.0f, fullWidth);

		ImGui::Text("Frame Time: %.2f ms", ms);
		ImGui::PlotLines("##MS", msHistory, HISTORY_LENGTH, offset, nullptr, 0.0f, 40.0f, fullWidth);

		ImGui::End();
	}
}
