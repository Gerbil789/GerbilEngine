#include "enginepch.h"
#include "StatisticsWindow.h"
#include "imgui/imgui.h"
#include "Engine/Renderer/Renderer2D.h"


namespace Engine
{

	void StatisticsWindow::OnImGuiRender()
	{
		ImGui::Begin("Statistics");
		ImGui::Separator();
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Separator();
		ImGui::Text("FPS: %d", static_cast<int>(Application::Get().fps));
		ImGui::End();
	}
}