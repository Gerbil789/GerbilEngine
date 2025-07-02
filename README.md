[![Debug Build](https://github.com/Gerbil789/GerbilEngine/actions/workflows/wokrflow.yml/badge.svg?job=build-debug)](https://github.com/Gerbil789/GerbilEngine/actions/workflows/wokrflow.yml)
[![Release Build](https://github.com/Gerbil789/GerbilEngine/actions/workflows/wokrflow.yml/badge.svg?job=build-release)](https://github.com/Gerbil789/GerbilEngine/actions/workflows/wokrflow.yml)
[![License](https://img.shields.io/badge/License-MIT-blue)](#license "Go to license section")
[![GH Pages Deploy](https://github.com/MichaelCurrin/badge-generator/workflows/GH%20Pages%20Deploy/badge.svg)](https://github.com/MichaelCurrin/badge-generator/actions/workflows/main.yml "GitHub Actions workflow status")

# GerbilEngine
WebGPU based engine

#### Prerequisites
 - Windows machine
 - Visual Studio 2022

#### Setup
1. **Clone the Project**
- ```git clone --recursive https://github.com/Gerbil789/GerbilEngine.git```
2. **Generate project files**
- Navigate to the **Scripts** Folder
- Run `Win-GenerateProjects.bat` 
3. **Build project in Visual Studio**
- Open solution in VS (`GerbilEngine.sln`)
- Find **`Editor`** project in Solution Explorer
- Right click -> **Build**
4. **Run**
- Right click **`Editor`** -> Set as Startup Project
- Run it
