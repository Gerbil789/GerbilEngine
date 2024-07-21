#include "EditorLayer.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Renderer/FrameBuffer.h"

namespace Engine
{
    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.f / 720.f, true) {}

    void EditorLayer::OnAttach()
    {
        ENGINE_PROFILE_FUNCTION();

        //load textures
        m_BackroundTexture = Texture2D::Create("assets/textures/background.png");
        m_GerbilTexture = Texture2D::Create("assets/textures/gerbil.jpg");
        m_TileTexture = Texture2D::Create("assets/textures/tile.png");
        m_Spritesheet = Texture2D::Create("assets/textures/spritesheet.png");
        for (int i = 0; i < 9; i++) {
            for (int j = 8; j > 5; j--) {
                m_TileTextures.push_back(SubTexture2D::CreateFromCoords(m_Spritesheet, { i, j }, { 18, 18 }));
            }
        }
        for (int i = 0; i < 9; i += 3) {
            for (int j = 0; j < 6; j += 3) {
                m_TileTextures.push_back(SubTexture2D::CreateFromCoords(m_Spritesheet, { i, j }, { 18, 18 }, { 3, 3 }));
            }
        }

        //create frame buffer
        FrameBufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = FrameBuffer::Create(fbSpec);

        //create scene
        m_ActiveScene = CreateRef<Scene>();

        m_SquareEntity = m_ActiveScene->CreateEntity("square");
        m_SquareEntity.AddComponent<SpriteRendererComponent>(Color::Magenta);


        m_CameraEntity = m_ActiveScene->CreateEntity("camera");
        m_CameraEntity.AddComponent<CameraComponent>();
    }

    void EditorLayer::OnDetach()
    {
        ENGINE_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        ENGINE_PROFILE_FUNCTION();

        //resize
        if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}




        //update camera controller if viewport is focused
        if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts);


        //clear frame buffer
        Renderer2D::ResetStats();
        m_FrameBuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();





        //Renderer2D::BeginScene(m_CameraController.GetCamera());


        //glm::vec3 randomPosition = { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX };
        //m_SquareEntity.GetComponent<TransformComponent>().Transform = glm::translate(glm::mat4(1.0f), randomPosition);

        //update scene
        m_ActiveScene->OnUpdate(ts);
      
        //Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 64.0f, 64.0f }, m_BackroundTexture, 8.0f, Color::White);  //Background
        //Renderer2D::EndScene();

        //tiles
        /*Renderer2D::BeginScene(m_CameraController.GetCamera());
        for (int i = 0; i < 9; i++) {
            Renderer2D::DrawQuad({ (float)i , -2.0f }, { 1.0f, 1.0f }, m_TileTextures[17], 1.0f, Color::Gray);
        }

        for (int i = 0; i < 9; i++) {
            Renderer2D::DrawQuad({ (float)i , -1.0f }, { 1.0f, 1.0f }, m_TileTextures[12], 1.0f, Color::DarkGray);
        }

        for (int i = 0; i < 9; i++) {
            Renderer2D::DrawQuad({ (float)i , 0.0f }, { 1.0f, 1.0f }, m_TileTextures[0], 1.0f, Color::Orange);
        }

        for (int i = 0; i < 9; i++) {
            Renderer2D::DrawQuad({ (float)i , 1.0f }, { 1.0f, 1.0f }, m_TileTextures[7], 1.0f, Color::Brown);
        }

        for (int i = 0; i < 3; i++) {
            Renderer2D::DrawQuad({ (float)i * 3 + 1 , 3.0f }, { 3.0f, 3.0f }, m_TileTextures[29], 1.0f, Color::White);
        }
        Renderer2D::EndScene();*/


        m_FrameBuffer->Unbind();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);
    }

    void EditorLayer::OnImGuiRender()
    {
        ENGINE_PROFILE_FUNCTION();

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

                if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        ImGui::Begin("Statistics");
        ImGui::Text("Draw Calls: %d", Renderer2D::GetStats().DrawCalls);
        ImGui::Text("Quads: %d", Renderer2D::GetStats().QuadCount);
        ImGui::Text("Vertices: %d", Renderer2D::GetStats().GetTotalVertexCount());
        ImGui::Text("Indices: %d", Renderer2D::GetStats().GetTotalIndexCount());

        ImGui::Separator();

        ImGui::DragFloat3("Camera Transform", glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3]));

        {
            auto& Camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;
            float orthoSize = Camera.GetOrthographicSize();
            if (ImGui::DragFloat("Camera Orthographic Size", &orthoSize, 0.1f)) 
            {
                Camera.SetOrthographicSize(orthoSize);
            }
        }

        ImGui::End();


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding
        ImGui::Begin("Viewport");

        m_ViewportFocused =  ImGui::IsWindowFocused();
        m_ViewportHovered =  ImGui::IsWindowHovered();

        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportSize.x, viewportSize.y };


        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{0, 1}, ImVec2{1, 0});

        ImGui::End();
        ImGui::PopStyleVar(); // Restore padding


        ImGui::End();
    }
}