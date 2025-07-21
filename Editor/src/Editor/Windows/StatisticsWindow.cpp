#include "enginepch.h"
#include "StatisticsWindow.h"
#include "Engine/Renderer/Renderer.h"
#include <imgui.h>

namespace Editor
{
	using namespace Engine;

	void StatisticsWindow::OnUpdate(Engine::Timestep ts)
	{
		ImGui::Begin("Statistics");
		//ImGui::Text("FPS: %d", static_cast<int>(Application::Get().GetFPS()));
		//ImGui::Separator();
		//auto stats2D = Renderer2D::GetStats();
		//ImGui::Text("2D");
		//ImGui::Text("Draw Calls: %d", stats2D.DrawCalls);
		//ImGui::Text("Quads: %d", stats2D.QuadCount);
		//ImGui::Text("Triangles: %d", stats2D.TriangleCount);
		//ImGui::Text("Vertices: %d", stats2D.GetTotalVertexCount());
		//ImGui::Text("Indices: %d", stats2D.GetTotalIndexCount());
		//ImGui::Separator();
		/*auto stats = Renderer::GetStats();
		ImGui::Text("3D");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Vertices: %d", stats.VertexCount);
		ImGui::Text("Indices: %d", stats.IndicesCount);*/
		ImGui::Text("3D");
		ImGui::Text("Draw Calls: %s", "undefined");
		ImGui::Text("Vertices: %s", "undefined");
		ImGui::Text("Indices: %s", "undefined");
		ImGui::End();
	}
}