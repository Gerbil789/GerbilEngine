#pragma once

namespace engine { class Window; }

namespace editor
{
  class EditorRenderer
  {
  public:
    static void Initialize(const engine::Window& window);
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame();
  };
}