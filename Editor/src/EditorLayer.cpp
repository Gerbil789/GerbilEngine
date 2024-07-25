#include "EditorLayer.h"
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Utils/PlatformUtils.h"
#include "Engine/Math/Math.h"

namespace Engine
{
    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.f / 720.f, false, false) {}

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
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);



      /*class CameraController : public ScriptableEntity
        {
        public:
            void OnCreate() {}

            void OnDestroy() {}

            void OnUpdate(Timestep ts) 
            {
                auto& position = GetComponent<TransformComponent>().Position;
				float speed = 5.0f;

				if (Input::IsKeyPressed(Key::A))
                    position.x -= speed * ts;
				if (Input::IsKeyPressed(Key::D))
                    position.x += speed * ts;
				if (Input::IsKeyPressed(Key::W))
                    position.y += speed * ts;
				if (Input::IsKeyPressed(Key::S))
                    position.y -= speed * ts;
            }

        };

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();*/

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

        //update scene
        m_ActiveScene->OnUpdate(ts);

        m_FrameBuffer->Unbind();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
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
                if(ImGui::MenuItem("New", "ctrl + N"))
				{
					NewScene();
				}

                if (ImGui::MenuItem("Open", "ctrl + O"))
                {
                    OpenScene();
                }

                if (ImGui::MenuItem("Save", "ctrl + S"))
                {
                    ENGINE_LOG_INFO("Save");
                }

                if (ImGui::MenuItem("Save as", "ctrl + shift + S"))
                {
                    SaveSceneAs();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        m_SceneHierarchyPanel.OnImGuiRender();
        ImGui::ShowDemoWindow();


        ImGui::Begin("Statistics");
        ImGui::Text("Draw Calls: %d", Renderer2D::GetStats().DrawCalls);
        ImGui::Text("Quads: %d", Renderer2D::GetStats().QuadCount);
        ImGui::Text("Vertices: %d", Renderer2D::GetStats().GetTotalVertexCount());
        ImGui::Text("Indices: %d", Renderer2D::GetStats().GetTotalIndexCount());
        ImGui::Separator();
        ImGui::End();


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding
        ImGui::Begin("Viewport");
        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportSize.x, viewportSize.y };
        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        //gizmo
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();

        if (selectedEntity && m_GizmoType != -1) 
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);


            auto cameraEntity = m_ActiveScene->GetMainCameraEntity();
            auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            const glm::mat4& cameraProjection = camera.GetProjection();
            glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());


            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapValue = 0.5f; // Snap to 0.5m for translation/scale
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f; // Snap to 45 degrees for rotation

            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 originalRotation = tc.Rotation;
                glm::vec3 deltaRotation = glm::degrees(rotation) - originalRotation;

				tc.Position = translation;
				tc.Rotation += deltaRotation; // to prevent gimbal lock
				tc.Scale = scale;
			}

        }

        ImGui::End();
        ImGui::PopStyleVar(); // Restore padding

        ImGui::End();
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.GetRepeatCount() > 0)
            return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		    case Key::N:
		    {
		    	if (control)
		    		NewScene();
		    	break;
		    }
		    case Key::O:
		    {
		    	if (control)
		    	{
		    		OpenScene();
		    	}
		    	break;
		    }
		    case Key::S:
		    {
		    	if (control && shift)
		    	{
		    		SaveSceneAs();
		    	}else if(control)
		    	{
                    ENGINE_LOG_INFO("Save (not implemented)");
		    	}
		    	break;
		    }
            

            case Key::Q:
				m_GizmoType = -1;
				break;

            case Key::W:
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;

            case Key::E:
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;

            case Key::R:
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
		}

    }
    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OpenScene()
	{
		std::string path = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
        if (!path.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(path);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
			ENGINE_LOG_INFO("Open file {0}", path);
		}
    }

	void EditorLayer::SaveSceneAs()
    {
		std::string path = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!path.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(path);
			ENGINE_LOG_INFO("Save as {0}", path);
		}
	}
}