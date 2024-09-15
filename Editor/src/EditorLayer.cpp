#include "EditorLayer.h"
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Math/Math.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Serializer.h"

#include "Windows/ContentBrowserWindow.h"
#include "Windows/InspectorWindow.h"
#include "Windows/SceneHierarchyWindow.h"
#include "Windows/MaterialWindow.h"
#include "Windows/SettingsWindow.h"
#include "Windows/StatisticsWindow.h"

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Renderer/Mesh.h" //temp

//TODO: clean up this file

namespace Engine
{
    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach()
    {
        ENGINE_PROFILE_FUNCTION();
        SceneManager::AddObserver(this);

        //load textures
        m_Icon_Play = AssetManager::GetAsset<Texture2D>("resources/icons/play.png");
        m_Icon_Pause = AssetManager::GetAsset<Texture2D>("resources/icons/pause.png");
        m_Icon_Next = AssetManager::GetAsset<Texture2D>("resources/icons/skip_next.png");

        //create editor frame buffer
        FrameBufferSpecification editorFrameBufferSpecification;
        editorFrameBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
        editorFrameBufferSpecification.Width = 1280;
        editorFrameBufferSpecification.Height = 720;
        m_EditorFrameBuffer = FrameBuffer::Create(editorFrameBufferSpecification);

		//create game frame buffer
        FrameBufferSpecification gameFrameBufferSpecification;
        gameFrameBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8 };
        gameFrameBufferSpecification.Width = 1280;
        gameFrameBufferSpecification.Height = 720;
        m_GameFrameBuffer = FrameBuffer::Create(gameFrameBufferSpecification);

        RenderCommand::SetClearColor({ 0.05f, 0.05f, 0.05f, 1.0f });
        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f); //TODO: what are these values? must it be there?
        SceneManager::CreateScene("untitledScene");

		m_EditorWindows.push_back(CreateRef<SceneHierarchyWindow>());
		m_EditorWindows.push_back(CreateRef<InspectorWindow>());
		m_EditorWindows.push_back(CreateRef<ContentBrowserWindow>());
		m_EditorWindows.push_back(CreateRef<MaterialWindow>());
		m_EditorWindows.push_back(CreateRef<SettingsWindow>());
		m_EditorWindows.push_back(CreateRef<StatisticsWindow>());


        // ---- testing -----
		Ref<Mesh> cube = AssetManager::GetAsset<Mesh>("assets/cube.fbx");
		ENGINE_LOG_INFO("Cube vertices: {0}", cube->GetVertexCount());
        ENGINE_LOG_INFO("Cube indices: {0}", cube->GetIndices().size());
        ENGINE_LOG_INFO("Cube normals: {0}", cube->GetNormals().size());
        ENGINE_LOG_INFO("Cube UVs: {0}", cube->GetUVs().size());

        //Ref<Mesh> skull = AssetManager::GetAsset<Mesh>("assets/skull.fbx");
		//ENGINE_LOG_INFO("Skull vertices: {0}", skull->GetVertexCount());
        //ENGINE_LOG_INFO("Skull indices: {0}", skull->GetIndices().size());
        //ENGINE_LOG_INFO("Skull normals: {0}", skull->GetNormals().size());
        //ENGINE_LOG_INFO("Skull UVs: {0}", skull->GetUVs().size());
    }

    void EditorLayer::OnDetach()
    {
        ENGINE_PROFILE_FUNCTION();
        SceneManager::RemoveObserver(this);
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        ENGINE_PROFILE_FUNCTION();
        m_CurrentScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

        //resize
        FrameBufferSpecification spec = m_EditorFrameBuffer->GetSpecification();
        if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_EditorFrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

        //clear frame buffer
        Renderer2D::ResetStats();
        m_EditorFrameBuffer->Bind();
        RenderCommand::Clear();
        m_EditorFrameBuffer->ClearAttachment(1, -1); //clear ID attachment

        //update scene
        m_EditorCamera.OnUpdate(ts);
        m_CurrentScene->OnUpdate(ts, m_EditorCamera);


		//hovered entity
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        
        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
            int pixelData = m_EditorFrameBuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity{ (entt::entity)pixelData, m_CurrentScene.get() };
		}

        m_EditorFrameBuffer->Unbind();




        ////////// GAME FRAMEBUFFER //////////

        //resize
        FrameBufferSpecification gameViewSpec = m_GameFrameBuffer->GetSpecification();
        if (m_GameViewSize.x > 0.0f && m_GameViewSize.y > 0.0f && (gameViewSpec.Width != m_GameViewSize.x || gameViewSpec.Height != m_GameViewSize.y))
        {
            m_GameFrameBuffer->Resize((uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
        }

        //clear frame buffer
        //Renderer2D::ResetStats();
        m_GameFrameBuffer->Bind();
        RenderCommand::Clear();

        //update scene
        m_CurrentScene->OnUpdate(ts);
        m_GameFrameBuffer->Unbind();
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
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen) { ImGui::PopStyleVar(2); }

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), dockspace_flags);
        }


        MenuBar();
		for (auto& window : m_EditorWindows)
		{
            window->OnImGuiRender();
		}
        //ImGui::ShowDemoWindow();

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
        uint32_t textureID = m_EditorFrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                //conver to string
                std::wstring ws(path);
                std::string str(ws.begin(), ws.end());


                ENGINE_LOG_INFO("Dropped file: {0}", str);

                //open scene
                if (str.find(".scene") != std::string::npos)
                {
                    SceneManager::LoadScene(str);
                }
            }
            ImGui::EndDragDropTarget();
        }


        //gizmo
        Entity selectedEntity = m_CurrentScene->GetSelectedEntity();

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


        ////////

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding
        ImGui::Begin("Game");


        auto gameViewSize = ImGui::GetContentRegionAvail();
        m_GameViewSize = { gameViewSize.x, gameViewSize.y };
        uint32_t GameViewtextureID = m_GameFrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)GameViewtextureID, ImVec2(m_GameViewSize.x, m_GameViewSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        ImGui::End();
        ImGui::PopStyleVar(); // Restore padding

        ImGui::End();
    }

    void EditorLayer::OnSceneChanged()
    {
        m_CurrentScene = SceneManager::GetCurrentScene();
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
                    SceneManager::CreateScene("New Scene");
		    	break;
		    }
		    case Key::O:
		    {
		    	if (control)
		    	{
                    SceneManager::LoadScene();
		    	}
		    	break;
		    }
		    case Key::S:
		    {
		    	if (control)
		    	{
                    if(shift)
					{
                        SceneManager::SaveSceneAs();
					}
					else
					{
						SceneManager::SaveScene();
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
            case Key::C:
			{
				if (control)
				{
                    m_CurrentScene->CopyEntity(m_CurrentScene->GetSelectedEntity());
				}
				break;
			}
            case Key::V:
            {
                if (control)
                {
                    m_CurrentScene->PasteEntity();
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
                m_CurrentScene->SelectEntity(m_HoveredEntity);
			}
		}
        return false;
    }

    void EditorLayer::OnScenePlay()
    {
        /*m_SceneState = Scene::SceneState::Play;

        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_InspectorPanel.SetContext(m_ActiveScene);*/
    }

    void EditorLayer::OnSceneStop()
	{
       /* m_ActiveScene->OnRuntimeStop();
        m_SceneState = Scene::SceneState::Edit;
        m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_InspectorPanel.SetContext(m_ActiveScene);*/
	}

    void EditorLayer::OnDuplicatedEntity()
    {
        auto selectedEntity = SceneManager::GetCurrentScene()->GetSelectedEntity();
        if (selectedEntity)
        {
            SceneManager::GetCurrentScene()->DuplicateEntity(selectedEntity);
        }
    }

    void EditorLayer::MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "ctrl + N"))
                {
                    SceneManager::CreateScene("New Scene");
                }

                if (ImGui::MenuItem("Open", "ctrl + O"))
                {
                    SceneManager::LoadScene();
                }

                if (ImGui::MenuItem("Save", "ctrl + S"))
                {
                    SceneManager::SaveScene();
                }

                if (ImGui::MenuItem("Save as", "ctrl + shift + S"))
                {
                    SceneManager::SaveSceneAs();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Settings")) { /*m_SettingsPanel.SetVisible(true);*/ }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

                ImGui::EndMenu();
            }

            // ---- BUTTONS ----
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 30);


            if (m_CurrentScene->IsPlaying())
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            }

            if (ImGui::ImageButton((ImTextureID)m_Icon_Play->GetRendererID(), { 20, 20 }, { 0, 1 }, { 1, 0 }))
            {
                if(m_CurrentScene->IsPlaying())
				{
					m_CurrentScene->OnStop();
				}
				else
				{
					m_CurrentScene->OnPlay();
				}

            }

            ImGui::PopStyleColor(1);
      


            if (ImGui::ImageButton((ImTextureID)m_Icon_Pause->GetRendererID(), { 20, 20 }, { 0, 1 }, { 1, 0 })) 
            {

            }

            if (ImGui::ImageButton((ImTextureID)m_Icon_Next->GetRendererID(), { 20, 20 }, { 0, 1 }, { 1, 0 })) 
            {
            
            }

            ImGui::EndMenuBar();
        }
    }
}