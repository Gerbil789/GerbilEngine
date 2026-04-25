#include "ViewportWindow.h"

#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include "Editor/Core/EditorPicker.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Windows/Viewport/ViewportCameraController.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/RenderPass/RenderPassRegistry.h"
#include "Engine/Core/Runtime.h"

#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Editor
{
	namespace
	{
		static Engine::Camera m_Camera;
		static EditorPicker* s_EntityPicker = nullptr;
		static ViewportCameraController s_Controller;

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		bool m_GizmoPreviouslyUsed = false;

		std::unordered_map<Engine::Entity, glm::mat4> m_InitialWorldTransforms;
		glm::mat4 m_InitialPrimaryWorld = glm::mat4(1.0f);

		static ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;
	}

	void ViewportWindow::Initialize()
	{
		ImGuizmo::AllowAxisFlip(true);
		ImGuizmo::SetGizmoSizeClipSpace(0.15f);

		m_Camera.SetBackground(Engine::Camera::Background::Skybox);
		m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -20.0f));

		s_Controller.Initialize(&m_Camera);
		Engine::g_Renderer.SetCamera(&m_Camera);

		Engine::g_Renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Shadow | Engine::RenderPassType::Opaque/* | Engine::RenderPassType::Normal | Engine::RenderPassType::Wireframe*/);

		s_EntityPicker = new EditorPicker();

		/*Engine::SceneManager::RegisterOnSceneChanged([this](Engine::Scene& scene)
			{
				SelectionManager::Clear(SelectionType::Entity);

				if (EditorRuntime::GetState() == EditorState::Edit)
				{
					Engine::g_Renderer.SetCamera(&m_Camera);
				}
				else if (EditorRuntime::GetState() == EditorState::Play)
				{
					auto cameraEntity = scene.GetActiveCamera();
					if (cameraEntity)
					{
						Engine::Camera* camera = cameraEntity.Get<Engine::CameraComponent>().camera;
						camera->SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
						Engine::g_Renderer.SetCamera(camera);
					}
				}

			});*/

		Engine::EventBus::Get().Subscribe<Engine::MouseButtonPressedEvent>([this](const Engine::MouseButtonPressedEvent& e)
			{
				if (EditorContext::state == EditorState::Play)
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

					Engine::Uuid uuid = s_EntityPicker->Pick(mx, my, Engine::g_Renderer.GetRenderContext());

					if (uuid)
					{
						Engine::Entity entity = Engine::SceneManager::GetActiveScene().GetEntity(uuid);
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
				if (EditorContext::state == EditorState::Play)
				{
					return;
				}

				if (e.GetKey() == Engine::KeyCode::Q) gizmoType = static_cast<ImGuizmo::OPERATION>(0);
				if (e.GetKey() == Engine::KeyCode::W) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
				if (e.GetKey() == Engine::KeyCode::E) gizmoType = ImGuizmo::OPERATION::ROTATE;
				if (e.GetKey() == Engine::KeyCode::R) gizmoType = ImGuizmo::OPERATION::SCALE;
			});
	}

	static void UpdateViewportSize()
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
					Engine::g_Renderer.SetColorTarget(colorTexture.createView(view));
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

					Engine::g_Renderer.SetDepthTarget(depthTexture.createView(view));
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

		Engine::Entity selectedEntity = Engine::SceneManager::GetActiveScene().GetEntity(selectedId);

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
				Engine::Entity entity = Engine::SceneManager::GetActiveScene().GetEntity(id);
				auto& tc = entity.Get<Engine::TransformComponent>();
				m_InitialWorldTransforms[entity] = tc.GetWorldMatrix();
			}

			m_InitialPrimaryWorld = m_InitialWorldTransforms[Engine::SceneManager::GetActiveScene().GetEntity(selectedId)];
		}

		if (isUsing)
		{

			glm::mat4 newPrimaryWorld = worldTransform;
			glm::mat4 delta = newPrimaryWorld * glm::inverse(m_InitialPrimaryWorld);

			for (auto id : SelectionManager::GetAll(SelectionType::Entity))
			{
				Engine::Entity entity = Engine::SceneManager::GetActiveScene().GetEntity(id);
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
				Engine::Entity entity = Engine::SceneManager::GetActiveScene().GetEntity(id);
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

			if (EditorContext::state == EditorState::Edit)
			{
				if (ImGui::Button("Play", ImVec2(buttonWidth, 0)))
				{
					Engine::Runtime::Start();
					EditorContext::state = EditorState::Play;

					SelectionManager::Clear(SelectionType::Entity);

					Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
					auto cameraEntity = scene.GetActiveCamera();
					if (cameraEntity)
					{
						Engine::Camera* camera = cameraEntity.Get<Engine::CameraComponent>().camera;
						camera->SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
						Engine::g_Renderer.SetCamera(camera);
					}
				}
			}
			else
			{
				if (ImGui::Button("Stop", ImVec2(buttonWidth, 0)))
				{
					Engine::Runtime::Stop();
					EditorContext::state = EditorState::Edit;
					Engine::g_Renderer.SetCamera(&m_Camera);
				}
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
				auto flags = Engine::g_Renderer.GetEnabledFlags();

				auto RenderPassToggle = [&](const char* label, Engine::RenderPassType flag) {
					// Check if the bit is currently set
					bool isEnabled = (flags & flag) != Engine::RenderPassType::None;

					if (ImGui::Checkbox(label, &isEnabled))
					{
						if (isEnabled)
							Engine::g_Renderer.EnableFlag(flag);
						else
							Engine::g_Renderer.DisableFlag(flag);
					}
					};

				RenderPassToggle("Background", Engine::RenderPassType::Background);
				RenderPassToggle("Opaque", Engine::RenderPassType::Opaque);
				RenderPassToggle("Light", Engine::RenderPassType::Light);
				RenderPassToggle("Shadow", Engine::RenderPassType::Shadow);
				RenderPassToggle("Normal", Engine::RenderPassType::Normal);
				RenderPassToggle("Wireframe", Engine::RenderPassType::Wireframe);

				ImGui::EndCombo();
			}
		}

		ImGui::EndChild();
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

		Engine::g_Renderer.RenderScene(&Engine::SceneManager::GetActiveScene());
		ImGui::Image(static_cast<WGPUTextureView>(Engine::g_Renderer.GetTextureView()), viewportSize);

		DrawOverlay(imagePos, viewportSize);
		DrawGizmos();

		ImGui::End();
	}
}