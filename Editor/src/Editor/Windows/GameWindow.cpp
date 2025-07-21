#include "GameWindow.h"
#include <imgui.h>

namespace Editor
{
	using namespace Engine;


	GameWindow::GameWindow()
	{
		//create game frame buffer
		//FrameBufferSpecification gameFrameBufferSpecification;
		//gameFrameBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8 };
		//gameFrameBufferSpecification.Width = 1280;
		//gameFrameBufferSpecification.Height = 720;
		//m_GameFrameBuffer = FrameBuffer::Create(gameFrameBufferSpecification);
	}
	void GameWindow::OnUpdate(Timestep ts)
	{
		////////// GAME FRAMEBUFFER //////////
		////resize
		//FrameBufferSpecification gameViewSpec = m_GameFrameBuffer->GetSpecification();
		//if (m_GameViewSize.x > 0.0f && m_GameViewSize.y > 0.0f && (gameViewSpec.Width != m_GameViewSize.x || gameViewSpec.Height != m_GameViewSize.y))
		//{
		//	m_GameFrameBuffer->Resize((uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
		//}
		
		////clear frame buffer
		////Renderer2D::ResetStats();
		//m_GameFrameBuffer->Bind();
		//RenderCommand::Clear();
		
		////update scene
		//m_CurrentScene->OnUpdate(ts);
		//m_GameFrameBuffer->Unbind();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding
		ImGui::Begin("Game");


		//auto gameViewSize = ImGui::GetContentRegionAvail();
		//m_GameViewSize = { gameViewSize.x, gameViewSize.y };
		//uint32_t GameViewtextureID = m_GameFrameBuffer->GetColorAttachmentRendererID();
		//ImGui::Image((void*)GameViewtextureID, ImVec2(m_GameViewSize.x, m_GameViewSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar(); // Restore padding
	}

}