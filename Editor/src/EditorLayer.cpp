#include "EditorLayer.h"
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Renderer/FrameBuffer.h"
//#include "Engine/Scene/SceneSerializer.h"
//#include "Engine/Utils/PlatformUtils.h"
#include "Engine/Math/Math.h"

namespace Engine
{
    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach()
    {
        ENGINE_PROFILE_FUNCTION();

        //load textures
        m_Icon_Play = Texture2D::Create("resources/icons/play.png");
        m_Icon_Stop = Texture2D::Create("resources/icons/stop.png");

        m_ShaderLibrary.Load("Texture", "assets/shaders/Texture.glsl");

        m_Material = CreateRef<Material>();
        m_Material->shaderName = "Texture";
        m_Material->texture = Texture2D::Create("assets/textures/gerbil.jpg");

        //create frame buffer
        FrameBufferSpecification fbSpec;
        fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = FrameBuffer::Create(fbSpec);

        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

        SceneManager::CreateScene("New Scene");
        m_SceneHierarchyPanel.SetContext(SceneManager::GetCurrentScene());
        m_InspectorPanel.SetContext(SceneManager::GetCurrentScene());

        //m_EditorScene = CreateRef<Scene>();
        //m_SceneHierarchyPanel.SetContext(m_EditorScene);
        //m_InspectorPanel.SetContext(m_EditorScene);
        //m_ActiveScene = m_EditorScene;
    }

    void EditorLayer::OnDetach()
    {
        ENGINE_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        ENGINE_PROFILE_FUNCTION();

        SceneManager::GetCurrentScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

        //resize
        if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
           
		}


        //clear frame buffer
        Renderer2D::ResetStats();
        m_FrameBuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();

        m_FrameBuffer->ClearAttachment(1, -1); //clear ID attachment

        //update scene
        switch (m_SceneState)
        {
            case Scene::SceneState::Edit:
                m_EditorCamera.OnUpdate(ts);
                SceneManager::GetCurrentScene()->OnUpdateEditor(ts, m_EditorCamera);
                //m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
                break;
            case Scene::SceneState::Play:
                SceneManager::GetCurrentScene()->OnUpdateRuntime(ts);
                //m_ActiveScene->OnUpdateRuntime(ts);
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
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity{ (entt::entity)pixelData, SceneManager::GetCurrentScene().get() };
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
        m_SceneHierarchyPanel.OnImGuiRender();
        m_InspectorPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();
        ImGui::ShowDemoWindow();

        { //stats
        ImGui::Begin("Statistics");
        ImGui::Text("Hovered entity: %s", m_HoveredEntity ? m_HoveredEntity.GetComponent<NameComponent>().Name.c_str() : "None");
        ImGui::Separator();
        auto stats = Renderer2D::GetStats();
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
        ImGui::Separator();
        ImGui::End();
        }
       


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
                    OpenScene(str);
                }
            }
            ImGui::EndDragDropTarget();
        }


        //gizmo
        Entity selectedEntity = SceneManager::GetCurrentScene()->GetSelectedEntity();

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
            case Key::C:
			{
				if (control)
				{
                    SceneManager::GetCurrentScene()->CopyEntity(SceneManager::GetCurrentScene()->GetSelectedEntity());
					//m_EditorScene->CopyEntity(m_ActiveScene->GetSelectedEntity());
				}
				break;
			}
            case Key::V:
            {
                if (control)
                {
                    SceneManager::GetCurrentScene()->PasteEntity();
                    //m_EditorScene->PasteEntity();
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
                SceneManager::GetCurrentScene()->SelectEntity(m_HoveredEntity);
                //m_ActiveScene->SelectEntity(m_HoveredEntity);
			}
		}
        return false;
    }

    void EditorLayer::NewScene()
    {
       /* if (m_SceneState != Scene::SceneState::Edit) {
            return;
        }

        if (m_ActiveScene != nullptr) {
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
            RenderCommand::Clear();
        }

        m_ActiveScene = CreateRef<Scene>();
        m_SceneHierarchyPanel.SetContext(m_EditorScene);
        m_InspectorPanel.SetContext(m_EditorScene);
        m_EditorScenePath = std::filesystem::path();*/
    }

    void EditorLayer::OpenScene()
	{
        SceneManager::LoadScene();

       /* if (m_SceneState == Scene::SceneState::Play)
        {
            OnSceneStop();
        }

		std::string path = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
        if (path.empty()) {
            ENGINE_LOG_ERROR("Failed to open file {0}", path);
            return;
        }
        OpenScene(path);*/
    }

    void EditorLayer::OpenScene(const std::string& filepath)
    {
        SceneManager::LoadScene(filepath);

        /*if (!filepath.empty())
        {
            Ref<Scene> newScene = CreateRef<Scene>();
            SceneSerializer serializer(newScene);
            ENGINE_LOG_INFO("Open file {0}", filepath);

            if (serializer.Deserialize(filepath)) 
            {
                m_EditorScene = newScene;
                m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

                m_SceneHierarchyPanel.SetContext(m_EditorScene);
                m_InspectorPanel.SetContext(m_EditorScene);

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
        }*/
    }

	void EditorLayer::SaveSceneAs()
    {
        SceneManager::SaveSceneAs();

		/*std::string path = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!path.empty())
		{
			SerializeScene(m_ActiveScene, path);
            m_EditorScenePath = path;
			ENGINE_LOG_INFO("Save as {0}", path);
		}*/
	}

    void EditorLayer::SaveScene()
    {
        SceneManager::SaveScene();

      /*  if(!m_EditorScenePath.empty())
		{
            SerializeScene(m_ActiveScene, m_EditorScenePath.string());
            ENGINE_LOG_INFO("Save {0}", m_EditorScenePath.string());
		}*/

    }

  //  void EditorLayer::SerializeScene(Ref<Scene> scene, const std::string& filepath)
  //  {
  //      SceneSerializer serializer(scene);
		//serializer.Serialize(filepath);
  //  }

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

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f);

            Ref<Texture2D> icon = m_SceneState == Scene::SceneState::Play ? m_Icon_Stop : m_Icon_Play;

            if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { 20, 20 }, { 0, 1 }, { 1, 0 }))
            {
                if (m_SceneState == Scene::SceneState::Edit)
                {
                    OnScenePlay();
                }
                else if (m_SceneState == Scene::SceneState::Play)
                {
                    OnSceneStop();
                }
            }

            ImGui::EndMenuBar();
        }
    }
}