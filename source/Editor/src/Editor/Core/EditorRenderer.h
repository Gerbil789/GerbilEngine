#pragma once

namespace Engine { class Window; }

namespace Editor
{
  class EditorRenderer
  {
  public:
    static void Initialize(const Engine::Window& window);
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame();
  };
}