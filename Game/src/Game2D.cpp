#include "Game2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>



Game2D::Game2D() : Layer("Game2D"), m_CameraController(1280.f / 720.f, true) {}

void Game2D::OnAttach() 
{
	ENGINE_PROFILE_FUNCTION();
	m_BackroundTexture = Engine::Texture2D::Create("assets/textures/background.png");
	m_GerbilTexture = Engine::Texture2D::Create("assets/textures/gerbil.jpg");
	m_TileTexture = Engine::Texture2D::Create("assets/textures/tile.png");
	m_Spritesheet = Engine::Texture2D::Create("assets/textures/spritesheet.png");

	for (int i = 0; i < 9; i++) {
		for (int j = 8; j > 5; j--) {
			m_TileTextures.push_back(Engine::SubTexture2D::CreateFromCoords(m_Spritesheet, { i, j }, { 18, 18 }));
		}
	}

	for (int i = 0; i < 9; i += 3) {
		for (int j = 0; j < 6; j += 3) {
			m_TileTextures.push_back(Engine::SubTexture2D::CreateFromCoords(m_Spritesheet, { i, j }, { 18, 18 }, {3, 3}));
		}
	}

    Engine::FrameBufferSpecification fbSpec;
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_FrameBuffer = Engine::FrameBuffer::Create(fbSpec);
		
}

void Game2D::OnDetach() 
{
	ENGINE_PROFILE_FUNCTION();
}

void Game2D::OnUpdate(Engine::Timestep ts) 
{
	ENGINE_PROFILE_FUNCTION();

	m_CameraController.OnUpdate(ts);
	
	Engine::Renderer2D::ResetStats();
	{
		ENGINE_PROFILE_SCOPE("Renderer::Clear");
        m_FrameBuffer->Bind();


		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();
	}

	{
		ENGINE_PROFILE_SCOPE("Renderer2D::BeginScene");

		//Background
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Engine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 64.0f, 64.0f }, m_BackroundTexture, 8.0f, Engine::Color::White());
		Engine::Renderer2D::EndScene();


		//tiles
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());


		for (int i = 0; i < 9; i++) {
			Engine::Renderer2D::DrawQuad({ (float)i , 0.0f }, { 1.0f, 1.0f }, m_TileTextures[0], 1.0f, Engine::Color::Orange());
		}

		for (int i = 0; i < 9; i++) {
			Engine::Renderer2D::DrawQuad({ (float)i , 1.0f }, { 1.0f, 1.0f }, m_TileTextures[7], 1.0f, Engine::Color::Brown());
		}

		for (int i = 0; i < 3; i++) {
			Engine::Renderer2D::DrawQuad({ (float)i * 3 + 1 , 3.0f }, { 3.0f, 3.0f }, m_TileTextures[29], 1.0f, Engine::Color::White());
		}

		Engine::Renderer2D::EndScene();

        m_FrameBuffer->Unbind();

		
	


	}
}

void Game2D::OnEvent(Engine::Event& e) 
{
	m_CameraController.OnEvent(e);
}

void Game2D::OnImGuiRender() 
{
	static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Separator();

            if (ImGui::MenuItem("Exit")) { Engine::Application::Get().Close(); }

        
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::Begin("Info");

    ImGui::Text("Draw Calls: %d", Engine::Renderer2D::GetStats().DrawCalls);
    ImGui::Text("Quads: %d", Engine::Renderer2D::GetStats().QuadCount);
    ImGui::Text("Vertices: %d", Engine::Renderer2D::GetStats().GetTotalVertexCount());
    ImGui::Text("Indices: %d", Engine::Renderer2D::GetStats().GetTotalIndexCount());


    uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)textureID, ImVec2(1280.0f, 720.0f));

    ImGui::End();

    ImGui::End();


}