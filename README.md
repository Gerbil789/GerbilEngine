[![Windows](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_windows.yaml/badge.svg)](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_windows.yaml)
[![Linux](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_linux.yaml/badge.svg)](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_linux.yaml)

# GerbilEngine
My custom WebGPU based game engine

#### Prerequisites
 - Windows machine (linux support is comming)
 - Visual Studio 2022
   - C++ 23

#### Setup
1. **Clone the Project**
   - ```git clone https://github.com/Gerbil789/GerbilEngine.git```
2. **Generate project files**
   - Navigate to the **Scripts** directory
   - Run `Win-GenerateProjects.bat` 
3. **Build project in Visual Studio**
   - Open solution in VS (`GerbilEngine.sln`)
   - Find **`Editor`** project in Solution Explorer
   - Right click -> **Build**
4. **Run**

#### Used Libraries

| Library                                                                          | Purpose                          |
| -------------------------------------------------------------------------------- | -------------------------------- |
| **[Dawn](https://dawn.googlesource.com/dawn)**                                   | WebGPU implementation            |
| **[EnTT](https://github.com/skypjack/entt)**                                     | Entity Component System          |
| **[GLFW](https://www.glfw.org/)**                                                | Windowing and input handling     |
| **[GLM](https://github.com/g-truc/glm)**                                         | Mathematics (vectors, matrices)  |
| **[ImGui](https://github.com/ocornut/imgui)**                                    | Immediate-mode UI                |
| **[ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)**                      | 3D transform gizmo for ImGui     |
| **[portable-file-dialogs](https://github.com/samhocevar/portable-file-dialogs)** | Native file dialogs              |
| **[spdlog](https://github.com/gabime/spdlog)**                                   | Fast logging library             |
| **[tinygltf](https://github.com/syoyo/tinygltf)**                                | glTF mesh loading and processing |
| **[yaml-cpp](https://github.com/jbeder/yaml-cpp)**                               | YAML serialization/deserialization |