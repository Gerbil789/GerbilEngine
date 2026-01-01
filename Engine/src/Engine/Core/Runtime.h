#pragma once

namespace Engine
{
  class Runtime
  {
  public:
    Runtime();
    ~Runtime();

    void Initialize();
    void Run();
    void Shutdown();

  private:
    void LoadGameModule(const std::string& path);

    bool m_Running = false;
  };

}