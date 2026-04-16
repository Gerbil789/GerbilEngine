#include "ViewportWindow.h"

#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include "Editor/Core/EditorPicker.h"
#include "Editor/Core/EditorRuntime.h"
#include "Editor/Windows/Viewport/ViewportCameraController.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/RenderPass/WireframePass.h"
#include "Engine/Graphics/RenderPass/NormalPass.h"
#include "Engine/Graphics/RenderPass/LightPass.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"

#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Editor
{
	namespace
	{
		Engine::Scene* m_Scene = nullptr;

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		bool m_GizmoPreviouslyUsed = false;

		std::unordered_map<Engine::Entity, glm::mat4> m_InitialWorldTransforms;
		glm::mat4 m_InitialPrimaryWorld = glm::mat4(1.0f);

		static ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;

		static Engine::OpaquePass* s_OpaquePass = nullptr;
		static Engine::WireframePass* s_WireframePass = nullptr;
		static Engine::NormalPass* s_NormalPass = nullptr;
		static Engine::LightPass* s_LightPass = nullptr;
		static Engine::ShadowPass* s_ShadowPass = nullptr;

		static Engine::Camera m_Camera;
		static Engine::Renderer m_Renderer;

		static bool enableOpaquePass = true;
		static bool enableNormalPass = false;
		static bool enableWireframePass = false;
		static bool enableLightPass = false;

		static EditorPicker* s_EntityPicker = nullptr;
	}

	void ViewportWindow::UpdateViewportSize()
	{
		ImVec2 newSize = ImGui::GetContentRegionAvail();

		if (newSize.x != m_ViewportSize.x || newSize.y != m_ViewportSize.y)
		{
			m_ViewportSize = { newSize.x, newSize.y };
			m_Camera.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);

			if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
			{
				wgpu::Extent3D size = { static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y), 1 };

				// Color
				{
					wgpu::TextureDescriptor desc;
					desc.label = { "RendererColorTexture", WGPU_STRLEN };
					desc.dimension = wgpu::TextureDimension::_2D;
					desc.format = wgpu::TextureFormat::RGBA8Unorm;
					desc.size = size;
					desc.mipLevelCount = 1;
					desc.sampleCount = 1;
					desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
					wgpu::Texture colorTexture = Engine::GraphicsContext::GetDevice().createTexture(desc);

					wgpu::TextureViewDescriptor view;
					view.label = { "RendererColorTextureView", WGPU_STRLEN };
					view.dimension = wgpu::TextureViewDimension::_2D;
					view.format = desc.format;
					view.baseMipLevel = 0;
					view.mipLevelCount = 1;
					view.baseArrayLayer = 0;
					view.arrayLayerCount = 1;

					m_Renderer.SetColorTarget(colorTexture.createView(view));
				}

				// Depth
				{
					wgpu::TextureDescriptor desc;
					desc.label = { "RendererDepthTextureView", WGPU_STRLEN };
					desc.dimension = wgpu::TextureDimension::_2D;
					desc.format = wgpu::TextureFormat::Depth24Plus;
					desc.mipLevelCount = 1;
					desc.sampleCount = 1;
					desc.size = size;
					desc.usage = wgpu::TextureUsage::RenderAttachment;
					desc.viewFormatCount = 1;
					desc.viewFormats = &wgpu::TextureFormat::Depth24Plus;
					wgpu::Texture depthTexture = Engine::GraphicsContext::GetDevice().createTexture(desc);

					wgpu::TextureViewDescriptor view;
					view.aspect = wgpu::TextureAspect::DepthOnly;
					view.baseArrayLayer = 0;
					view.arrayLayerCount = 1;
					view.baseMipLevel = 0;
					view.mipLevelCount = 1;
					view.dimension = wgpu::TextureViewDimension::_2D;
					view.format = wgpu::TextureFormat::Depth24Plus;

					m_Renderer.SetDepthTarget(depthTexture.createView(view));
				}

				// Entity picker
				{
					s_EntityPicker->Resize(size.width, size.height);
				}
			}
		}

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
	}

	void DrawGizmos()
	{
		if (gizmoType == 0)
		{
			return;
		}

		auto selectedId = SelectionManager::GetPrimary(SelectionType::Entity);
		if (!selectedId)
		{
			return;
		}

		Engine::Entity selectedEntity = m_Scene->GetEntity(selectedId);

		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		const glm::mat4& cameraProjection = m_Camera.GetProjectionMatrix();
		glm::mat4 cameraView = m_Camera.GetViewMatrix();

		auto& transformComponent = selectedEntity.Get<Engine::TransformComponent>();
		glm::mat4 worldTransform = transformComponent.GetWorldMatrix();

		float* snapValue = nullptr;
		if (Engine::Input::IsKeyDown(Engine::KeyCode::LeftControl))
		{
			static float snapTranslateScale[3] = { 0.5f, 0.5f, 0.5f };
			static float snapRotate[3] = { 45.0f, 45.0f, 45.0f };
			snapValue = (gizmoType == ImGuizmo::OPERATION::ROTATE) ? snapRotate : snapTranslateScale;
		}

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), gizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(worldTransform), nullptr, snapValue);

		bool isUsing = ImGuizmo::IsUsing();

		if (isUsing && !m_GizmoPreviouslyUsed)
		{
			m_InitialWorldTransforms.clear();

			auto selection = SelectionManager::GetAll(SelectionType::Entity);

			for (auto id : selection)
			{
				Engine::Entity entity = m_Scene->GetEntity(id);
				auto& tc = entity.Get<Engine::TransformComponent>();
				m_InitialWorldTransforms[entity] = tc.GetWorldMatrix();
			}

			m_InitialPrimaryWorld = m_InitialWorldTransforms[m_Scene->GetEntity(selectedId)];
		}

		if (isUsing)
		{

			glm::mat4 newPrimaryWorld = worldTransform;
			glm::mat4 delta = newPrimaryWorld * glm::inverse(m_InitialPrimaryWorld);

			for (auto id : SelectionManager::GetAll(SelectionType::Entity))
			{
				Engine::Entity entity = m_Scene->GetEntity(id);
				auto& tc = entity.Get<Engine::TransformComponent>();

				glm::mat4 originalWorld = m_InitialWorldTransforms[entity];
				glm::mat4 newWorld = delta * originalWorld;

				glm::mat4 parentWorld = glm::mat4(1.0f);
				if (tc.parent)
				{
					parentWorld = tc.parent.Get<Engine::TransformComponent>().GetWorldMatrix();
				}

				glm::mat4 newLocal = glm::inverse(parentWorld) * newWorld;
				glm::vec3 rot;
				glm::vec3 trans, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));

				tc.position = trans;
				tc.rotation = rot;
				tc.scale = scale;
			}
		}

		if (!isUsing && m_GizmoPreviouslyUsed)
		{
			auto selection = SelectionManager::GetAll(SelectionType::Entity);

			std::vector<TransformData> before, after;

			for (auto& [entity, initialWorld] : m_InitialWorldTransforms)
			{
				auto& tc = entity.Get<Engine::TransformComponent>();
				{
					glm::mat4 parentWorld = glm::mat4(1.0f);
					if (tc.parent)
					{
						parentWorld = tc.parent.Get<Engine::TransformComponent>().GetWorldMatrix();
					}
					glm::mat4 initialLocal = glm::inverse(parentWorld) * initialWorld;
					glm::vec3 rot;
					glm::vec3 trans, scale;
					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(initialLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));
					before.push_back({ trans, rot, scale });
				}
			}

			std::vector<Engine::Entity> entities;
			entities.reserve(selection.size());

			for (auto id : selection)
			{
				Engine::Entity entity = m_Scene->GetEntity(id);
				entities.push_back(entity);
				auto& tc = entity.Get<Engine::TransformComponent>();
				TransformData afterData;
				afterData.Position = tc.position;
				afterData.Rotation = tc.rotation;
				afterData.Scale = tc.scale;
				after.push_back(afterData);
			}

			EditorCommandManager::Enqueue(std::make_unique<TransformEntitiesCommand>(entities, before, after));
		}

		m_GizmoPreviouslyUsed = isUsing;
	}

	void DrawOverlay(const ImVec2& imagePos, const ImVec2& size)
	{
		const float overlayHeight = 32.0f; // or 40.0f if you want more space

		ImGui::SetCursorPos(imagePos);

		ImGui::SetNextWindowBgAlpha(0.1f);

		ImGui::BeginChild("##OverlayUI",
			ImVec2(size.x, overlayHeight), // <-- ONLY TOP STRIP
			false,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings
		);

		// ---- CENTER BUTTON ----
		{
			float buttonWidth = 60.0f;

			ImGui::SetCursorPos(ImVec2(
				size.x * 0.5f - buttonWidth * 0.5f,
				4.0f
			));

			if (EditorRuntime::GetState() == EditorState::Edit)
			{
				if (ImGui::Button("Play", ImVec2(buttonWidth, 0)))
					EditorRuntime::SetEditorState(EditorState::Play);
			}
			else
			{
				if (ImGui::Button("Stop", ImVec2(buttonWidth, 0)))
					EditorRuntime::SetEditorState(EditorState::Edit);
			}
		}

		// ---- RIGHT COMBO ----
		{
			float comboWidth = 120.0f;
			float margin = 8.0f;

			ImGui::SetCursorPos(ImVec2(
				size.x - comboWidth - margin,
				4.0f
			));

			ImGui::SetNextItemWidth(comboWidth);

			if (ImGui::BeginCombo("##ViewportOptions", "Passes"))
			{
				if (ImGui::Checkbox("Opaque", &enableOpaquePass))
					enableOpaquePass ? m_Renderer.AddPass(s_OpaquePass) : m_Renderer.RemovePass(s_OpaquePass);

				if (ImGui::Checkbox("Light", &enableLightPass))
					enableLightPass ? m_Renderer.AddPass(s_LightPass) : m_Renderer.RemovePass(s_LightPass);

				if (ImGui::Checkbox("Normal", &enableNormalPass))
					enableNormalPass ? m_Renderer.AddPass(s_NormalPass) : m_Renderer.RemovePass(s_NormalPass);

				if (ImGui::Checkbox("Wireframe", &enableWireframePass))
					enableWireframePass ? m_Renderer.AddPass(s_WireframePass) : m_Renderer.RemovePass(s_WireframePass);

				ImGui::EndCombo();
			}
		}

		ImGui::EndChild();
	}

	void ViewportWindow::Initialize()
	{
		ImGuizmo::AllowAxisFlip(true);
		ImGuizmo::SetGizmoSizeClipSpace(0.15f);

		m_Camera.SetBackground(Engine::Camera::Background::Skybox);
		m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -20.0f));

		SetCameraController(new ViewportCameraController(&m_Camera)); //TODO: this api is weird, make controller static?

		m_Renderer.Initialize();
		m_Renderer.SetCamera(&m_Camera);

		m_Renderer.AddPass(new Engine::BackgroundPass());

		s_ShadowPass = new Engine::ShadowPass();
		m_Renderer.AddPass(s_ShadowPass);

		s_OpaquePass = new Engine::OpaquePass();
		m_Renderer.AddPass(s_OpaquePass);

		s_LightPass = new Engine::LightPass();
		s_NormalPass = new Engine::NormalPass();
		s_WireframePass = new Engine::WireframePass();

		s_EntityPicker = new EditorPicker();

		Engine::SceneManager::RegisterOnSceneChanged([this](Engine::Scene* scene)
			{
				m_Scene = scene;
				SelectionManager::Clear(SelectionType::Entity);

				if (EditorRuntime::GetState() == EditorState::Edit)
				{
					m_Renderer.SetCamera(&m_Camera);
				}
				else if (EditorRuntime::GetState() == EditorState::Play)
				{
					auto cameraEntity = m_Scene->GetActiveCamera();
					if (cameraEntity)
					{
						Engine::Camera* camera = cameraEntity.Get<Engine::CameraComponent>().camera;

						
						camera->SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
						m_Renderer.SetCamera(camera);
					}
				}
					
			});

		Engine::EventBus::Get().Subscribe<Engine::MouseButtonPressedEvent>([this](const Engine::MouseButtonPressedEvent& e)
			{
				if (EditorRuntime::GetState() == EditorState::Play)
				{
					Engine::Input::SetCursorMode(Engine::Input::CursorMode::Disabled);
					return;
				}

				if (e.GetMouseButton() == Engine::MouseCode::ButtonLeft && !ImGuizmo::IsOver())
				{
					ImVec2 mousePos = ImGui::GetMousePos();
					if (mousePos.x < m_ViewportBounds[0].x || mousePos.y < m_ViewportBounds[0].y || mousePos.x > m_ViewportBounds[1].x || mousePos.y > m_ViewportBounds[1].y)
					{
						return; 
					}

					const uint32_t mx = static_cast<uint32_t>(mousePos.x - m_ViewportBounds[0].x);
					const uint32_t my = static_cast<uint32_t>(mousePos.y - m_ViewportBounds[0].y);

					Engine::Uuid uuid = s_EntityPicker->Pick(mx, my, m_Renderer.GetRenderContext());

					if (uuid)
					{
						Engine::Entity entity = m_Scene->GetEntity(uuid);
						SelectionManager::Select(SelectionType::Entity, entity.GetUUID(), Engine::Input::IsKeyDown(Engine::KeyCode::LeftControl) || Engine::Input::IsKeyDown(Engine::KeyCode::LeftShift));
					}
					else
					{
						SelectionManager::Clear(SelectionType::Entity);
					}
				}
			});

		Engine::EventBus::Get().Subscribe<Engine::KeyPressedEvent>([this](const Engine::KeyPressedEvent& e)
			{
				if (EditorRuntime::GetState() == EditorState::Play)
				{
					return;
				}

				if (e.GetKey() == Engine::KeyCode::Q) gizmoType = static_cast<ImGuizmo::OPERATION>(0);
				if (e.GetKey() == Engine::KeyCode::W) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
				if (e.GetKey() == Engine::KeyCode::E) gizmoType = ImGuizmo::OPERATION::ROTATE;
				if (e.GetKey() == Engine::KeyCode::R) gizmoType = ImGuizmo::OPERATION::SCALE;
			});
	}

	void ViewportWindow::Draw()
	{
		ScopedStyle style
		{
			{ ImGuiStyleVar_WindowPadding, ImVec2(0, 0) },
			{ ImGuiCol_WindowBg, ImVec4(0.9f, 0.2f, 1.0f, 1.0f) },
			{ ImGuiStyleVar_WindowBorderSize, 0.0f }
		};

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		UpdateViewportSize();

		ImVec2 imagePos = ImGui::GetCursorPos();
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		m_Renderer.RenderScene(m_Scene);
		ImGui::Image(static_cast<WGPUTextureView>(m_Renderer.GetTextureView()), viewportSize);

		DrawOverlay(imagePos, viewportSize);
		DrawGizmos();

		ImGui::End();
	}

	Engine::Renderer& ViewportWindow::GetRenderer()
	{
		return m_Renderer;
	}
}