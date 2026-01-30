[![Windows](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_windows.yaml/badge.svg)](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_windows.yaml)
<!-- [![Linux](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_linux.yaml/badge.svg)](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_linux.yaml) -->

# GerbilEngine
My custom WebGPU based game engine

#### Prerequisites
 - Windows machine (linux support is comming)
 - Visual Studio 2022 (C++ 23)

#### Setup
1. **Clone the Project**
   - ```git clone https://github.com/Gerbil789/GerbilEngine.git```
  - *Navigate into the **Scripts** directory*
2. **Build premake**
	 - Run `build_premake.bat`
3. **Generate project files**
	 - Run `generate_project.bat` 
4. **Build project in Visual Studio**
   - Open solution in VS (`GerbilEngine.sln`)
   - Find **`Editor`** project in Solution Explorer
   - Right click -> **Build**
5. **Run**

#### Dependencies

*All dependencies are self contained in this repository*

| Library                                                                          | Purpose                          |
| -------------------------------------------------------------------------------- | -------------------------------- |
| **[Premake](https://premake.github.io)**                                   			 | Build system           					|
| **[Dawn](https://dawn.googlesource.com/dawn)**                                   | WebGPU implementation            |
| **[GLFW](https://www.glfw.org)**                                                 | Windowing and input handling     |
| **[GLM](https://github.com/g-truc/glm)**                                         | Mathematics 											|
| **[ImGui](https://github.com/ocornut/imgui)**                                    | Immediate-mode UI                |
| **[ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)**                      | 3D transform gizmo for ImGui     |
| **[EnTT](https://github.com/skypjack/entt)**                                     | Entity Component System          |
| **[spdlog](https://github.com/gabime/spdlog)**                                   | Fast logging library        		  |
| **[tinygltf](https://github.com/syoyo/tinygltf)**                                | glTF mesh loading and processing |
| **[yaml-cpp](https://github.com/jbeder/yaml-cpp)**                               | YAML serialization							  |
| **[miniaudio](https://github.com/mackron/miniaudio)**                            | Audio playback    							  |
| **[portable-file-dialogs](https://github.com/samhocevar/portable-file-dialogs)** | Native file dialogs              |


---

<img width="1920" height="1042" alt="screenshot" src="https://github.com/user-attachments/assets/1fc200c0-f796-463f-b10e-d2c8e8855740" />
