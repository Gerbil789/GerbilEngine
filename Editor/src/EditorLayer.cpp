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
    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach()
    {
        ENGINE_PROFILE_FUNCTION();

        //load textures
        m_BackroundTexture = Texture2D::Create("assets/textures/background.png");
        m_GerbilTexture = Texture2D::Create("assets/textures/gerbil.jpg");
        m_TileTexture = Texture2D::Create("assets/textures/tile.png");
        m_Spritesheet = Texture2D::Create("assets/textures/spritesheet.png");
        m_Icon_Play = Texture2D::Create("resources/icons/play.png");
        m_Icon_Stop = Texture2D::Create("resources/icons/stop.png");
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
        fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = FrameBuffer::Create(fbSpec);

        //create scene
        m_ActiveScene = CreateRef<Scene>();

        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

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
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}


        //clear frame buffer
        Renderer2D::ResetStats();
        m_FrameBuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();

        m_FrameBuffer->ClearAttachment(1, -1); //clear red integer attachment

        //update scene
        switch (m_SceneState)
        {
            case Engine::EditorLayer::SceneState::Edit:
                m_EditorCamera.OnUpdate(ts);
                m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
                break;
            case Engine::EditorLayer::SceneState::Play:
                m_ActiveScene->OnUpdateRuntime(ts);
                break;
            default:
                ENGINE_LOG_WARNING("Unknown scene state");
                break;
        }

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        
        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
            int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity{ (entt::entity)pixelData, m_ActiveScene.get() };
		}

        m_FrameBuffer->Unbind();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_EditorCamera.OnEvent(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
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
        m_ContentBrowserPanel.OnImGuiRender();

        ImGui::ShowDemoWindow();

        ImGui::Begin("Statistics");
        ImGui::Text("Hovered entity: %s", m_HoveredEntity ? m_HoveredEntity.GetComponent<NameComponent>().Name.c_str() : "None");

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
        ImGui::Separator();
        ImGui::End();


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding
        ImGui::Begin("Viewport");

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos(); 

        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportSize.x, viewportSize.y };
        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        if(ImGui::BeginDragDropTarget())
		{
			if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
                //conver to string
                std::wstring ws(path);
                std::string str(ws.begin(), ws.end());


				ENGINE_LOG_INFO("Dropped file: {0}", str);

                //open scene
                if(str.find(".scene") != std::string::npos)
				{
                   OpenScene(str);
				}
			}
			ImGui::EndDragDropTarget();
		}


        //gizmo
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();

        if (selectedEntity && m_GizmoType != -1) 
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
 
            ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

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

        UI_Toolbar();

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
		    	if (control)
		    	{
                    if(shift)
					{
						SaveSceneAs();
					}
					else
					{
						SaveScene();
					}
		    	}
		    	break;
		    }
            case Key::D:
                {
				if (control)
				{
					OnDuplicatedEntity();
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
    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if(e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if(m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
		}
        return false;
    }

    void EditorLayer::NewScene()
    {
        if (m_SceneState != SceneState::Edit) {
            return;
        }
        m_EditorScene = CreateRef<Scene>();
        m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        m_EditorScenePath = std::filesystem::path();
    }

    void EditorLayer::OpenScene()
	{
        if (m_SceneState == SceneState::Play)
        {
            OnSceneStop();
        }
            

		std::string path = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
        if (path.empty()) {
            ENGINE_LOG_ERROR("Failed to open file {0}", path);
            return;
        }
        OpenScene(path);
    }

    void EditorLayer::OpenScene(const std::string& filepath)
    {
        if (!filepath.empty())
        {
            Ref<Scene> newScene = CreateRef<Scene>();
            SceneSerializer serializer(newScene);
            ENGINE_LOG_INFO("Open file {0}", filepath);

            if (serializer.Deserialize(filepath)) 
            {
                m_EditorScene = newScene;
                m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

                m_SceneHierarchyPanel.SetContext(m_EditorScene);

                m_ActiveScene = m_EditorScene;
                m_EditorScenePath = filepath;
            }
            else 
            {
                ENGINE_LOG_ERROR("Failed to deserialize file {0}", filepath);
            }
        }
        else 
        {
            ENGINE_LOG_ERROR("File path is empty");
        }
    }

	void EditorLayer::SaveSceneAs()
    {
		std::string path = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!path.empty())
		{
			SerializeScene(m_ActiveScene, path);
            m_EditorScenePath = path;
			ENGINE_LOG_INFO("Save as {0}", path);
		}
	}

    void EditorLayer::SaveScene()
    {
        if(!m_EditorScenePath.empty())
		{
            SerializeScene(m_ActiveScene, m_EditorScenePath.string());
            ENGINE_LOG_INFO("Save {0}", m_EditorScenePath.string());
		}

    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::string& filepath)
    {
        SceneSerializer serializer(scene);
		serializer.Serialize(filepath);
    }

    void EditorLayer::UI_Toolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1));

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        Ref<Texture2D> icon = m_SceneState == SceneState::Play ? m_Icon_Stop : m_Icon_Play;
        ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x * 0.5f - 24.0f * 0.5f);

        if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { 24, 24 }, { 0, 1 }, { 1, 0 }))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}

		ImGui::End();

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
    }

    void EditorLayer::OnScenePlay()
    {
        m_SceneState = SceneState::Play;

        m_RuntimeScene = Scene::Copy(m_ActiveScene);
        m_RuntimeScene->OnRuntimeStart();
        m_ActiveScene = m_RuntimeScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnSceneStop()
	{
        m_RuntimeScene->OnRuntimeStop();

        m_SceneState = SceneState::Edit;

        
        m_RuntimeScene = nullptr;
        m_ActiveScene = m_EditorScene;

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

    void EditorLayer::OnDuplicatedEntity()
    {
        if(m_SceneState != SceneState::Edit)
		{
            return;
        }

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity)
        {
            m_EditorScene->DuplicateEntity(selectedEntity);
        }

        
    }
}