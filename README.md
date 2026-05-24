[![Windows](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_windows.yaml/badge.svg)](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_windows.yaml)
<!-- [![Linux](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_linux.yaml/badge.svg)](https://github.com/Gerbil789/GerbilEngine/actions/workflows/build_linux.yaml) -->

# GerbilEngine
WebGPU based game engine

#### Prerequisites
 - Windows machine
 - Visual Studio 2022 (C++ 23)

#### Setup
1. **Clone the Project**
	 - ```git clone https://github.com/Gerbil789/GerbilEngine.git```
2. **Generate project files**
	 - *Navigate into the **Scripts** directory*
	 - Run `generate_project.bat` 
3. **Build project in Visual Studio**
	 - Open solution in VS (`GerbilEngine.sln`)
	 - Right click Solution -> Build Solution
4. **Run**

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
| **[tinygltf](https://github.com/syoyo/tinygltf)**                                | glTF mesh processing             |
| **[glaze](https://github.com/stephenberry/glaze)**                               | Serialization							      |
| **[miniaudio](https://github.com/mackron/miniaudio)**                            | Audio playback    							  |
| **[RenderDoc](https://github.com/baldurk/renderdoc)** 													 | Graphics debbuger	              |

---

<img width="1920" height="1080" alt="screen1" src="https://github.com/user-attachments/assets/a8091422-2f4e-4f4f-926a-262a0accf6bb" />
<img width="1920" height="1080" alt="screen2" src="https://github.com/user-attachments/assets/657f1aae-249f-4058-8a5b-ba5dc1f6714d" />
<img width="1920" height="1080" alt="screen3" src="https://github.com/user-attachments/assets/8c2677f8-6cbd-4203-b420-5ef7cda494da" />
<img width="1920" height="1080" alt="screen4" src="https://github.com/user-attachments/assets/18990aaf-9cd5-454c-911c-605f600a84d3" />

