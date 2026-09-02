#include "Game.h"
#include "Engine/Core/Project.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Graphics/Font.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Event/WindowEvent.h"
#include "Engine/Core/Time.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Core/Log.h"
#include "Engine/Utility/Path.h"

// #include "Editor/Core/EditorContext.h"
// #include "Editor/Core/EditorWindowManager.h"
// #include "Editor/Command/EditorCommandManager.h"
// #include "Editor/Core/SelectionManager.h"
// #include "Editor/Core/EditorState.h"

// #include "Engine/Core/Runtime.h"

#include "Engine/Core/Configuration.h"

Game::Game()
{
  SetupWorkingDirectory();
  engine::Log::Initialize();

  engine::Project::Initialize(std::filesystem::current_path() / "projects/TestProject");

  engine::GraphicsContext::Initialize();
  GLFW::Initialize();

  m_Window.Initialize({"Game", 1280, 720, "resources/icons/logo.png"});
  m_Window.SetEventCallback([](auto &e)
                            { engine::EventBus::Publish(e); });

  engine::AssetManager::Initialize();

  engine::Input::Initialize(m_Window.Get());
  engine::Audio::Initialize();
  engine::FontManager::Initialize();

  engine::SceneManager::SetActiveScene(engine::Scene{10911691135274866464u});

  m_Renderer.Initialize();
  m_Renderer.SetFlags(engine::RenderPassType::Background | engine::RenderPassType::Shadow | engine::RenderPassType::Opaque);

  auto &scene = engine::AssetManager::GetAsset(engine::SceneManager::GetActiveScene());
  auto &registry = scene.GetRegistry();
  entt::entity cameraEntity = registry.view<engine::CameraComponent, engine::PrimaryCameraTag>(entt::exclude<engine::EditorTag>).front();
  auto &cc = registry.get<engine::CameraComponent>(cameraEntity);
  auto &tc = registry.get<engine::TransformComponent>(cameraEntity);
  m_Renderer.SetCamera(cc, tc);

  registry.emplace_or_replace<engine::TransformDirty>(cameraEntity);
  registry.emplace_or_replace<engine::CameraProjectionDirty>(cameraEntity);
  registry.emplace_or_replace<engine::CameraViewDirty>(cameraEntity);

  engine::EventBus::Subscribe<engine::WindowCloseEvent>([this](auto &)
                                                        {m_Running = false; LOG_INFO("Application closed"); return false; });

// #ifdef EDITOR
//   editor::editorContext.camera.background = Engine::CameraComponent::Background::Skybox;
//   editor::editorContext.camera.projectionType = Engine::CameraComponent::Projection::Perspective;
//   editor::editorContext.cameraTransform.position = glm::vec3{0.0f, 0.0f, -20.0f};

//   // Editor::editorContext.renderer = std::move(m_Renderer);
//   //  Editor::editorContext.renderer.Initialize();
//   editor::editorContext.renderer.SetFlags(Engine::RenderPassType::Background | Engine::RenderPassType::Shadow | Engine::RenderPassType::Opaque | Engine::RenderPassType::UI /* | Engine::RenderPassType::Normal | Engine::RenderPassType::Wireframe*/);
//   editor::editorContext.renderer.SetCamera(Editor::editorContext.camera, Editor::editorContext.cameraTransform);

//   EditorCommandManager::Initialize();
//   // EditorWindowManager::Initialize(m_Window);
//   SelectionManager::Initialize();

//   LOG_INFO("--- Editor initialization complete ---");
// #endif

  while (m_Running)
  {
    Update();
  }
}

Game::~Game()
{
// #ifdef EDITOR
//   EditorWindowManager::Shutdown();
// #endif

  engine::Audio::Shutdown();
  m_Window.Shutdown();
  GLFW::Shutdown();
  engine::GraphicsContext::Shutdown();
}

void Game::Update()
{
  if (m_Window.IsMinimized())
  {
    GLFW::WaitEvents();
    engine::Time::BeginFrame();
    return;
  }

  if (m_Window.SizeChanged())
  {
    // TODO: is static cast necessary?
    m_Window.ConfigureSurface();
    m_Renderer.SetSize(static_cast<float>(m_Window.GetWidth()), static_cast<float>(m_Window.GetHeight()));
    m_Window.ClearResizedFlag();
  }

  wgpu::Surface surface = m_Window.GetSurface();
  wgpu::SurfaceTexture surfaceTexture;

  surface.GetCurrentTexture(&surfaceTexture);
  if (!(surfaceTexture.status == wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal || surfaceTexture.status == wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal))
  {
    LOG_ERROR("Surface texture status is not optimal. status: {}", (int)surfaceTexture.status);
  }

  engine::Time::BeginFrame();
  engine::Input::Update();
  engine::Audio::Update();

  auto &scene = engine::AssetManager::GetAsset(engine::SceneManager::GetActiveScene());
  engine::TransformSystem::Update(scene);

  auto &registry = scene.GetRegistry();

  entt::entity cameraEntity = registry.view<engine::PrimaryCameraTag>().front();

  registry.emplace_or_replace<engine::TransformDirty>(cameraEntity);
  registry.emplace_or_replace<engine::CameraProjectionDirty>(cameraEntity);
  registry.emplace_or_replace<engine::CameraViewDirty>(cameraEntity);

  wgpu::TextureViewDescriptor colorViewDesc;
  colorViewDesc.label = "RendererColorTextureView";
  colorViewDesc.dimension = wgpu::TextureViewDimension::e2D;
  colorViewDesc.format = engine::GraphicsContext::GetSurfaceFormat();
  colorViewDesc.baseMipLevel = 0;
  colorViewDesc.mipLevelCount = 1;
  colorViewDesc.baseArrayLayer = 0;
  colorViewDesc.arrayLayerCount = 1;

  wgpu::TextureView targetColorView = surfaceTexture.texture.CreateView(&colorViewDesc);

  if (!targetColorView)
  {
    LOG_ERROR("Failed to create color texture view for surface texture");
    return;
  }

  m_Renderer.SetColorTarget(targetColorView);

  m_Renderer.RenderScene(scene);

  m_Window.GetSurface().Present();

// #ifdef EDITOR

//   EditorWindowManager::Update();
//   EditorCommandManager::ExecuteDeferredCommands();

//   if (Editor::editorContext.editorMode == EditorMode::Play)
//   {
//     Engine::Runtime::Update();
//   }

// #endif
}