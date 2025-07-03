workspace "GerbilEngine"
	architecture "x64"
	startproject "Editor"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

Includedir = {}
Includedir["glfw"] = "vendor/glfw/include" 					-- Windowing library
Includedir["dawn"] = "vendor/dawn/include" 					-- WebGPU implementation
Includedir["glm"] = "vendor/glm" 										-- Math library
Includedir["stb_image"] = "vendor/stb_image" 				-- Image loading library
Includedir["entt"] = "vendor/entt/include" 					-- ECS library
Includedir["yaml_cpp"] = "vendor/yaml-cpp/include" 	-- YAML parsing library
Includedir["assimp"] = "vendor/assimp/include" 			-- 3D model loading library
Includedir["ImGui"] = "vendor/imgui" 								-- ImGui library for GUI		
Includedir["ImGuizmo"] = "vendor/ImGuizmo" 					-- ImGuizmo for 3D manipulation

group "Dependencies"
	include "vendor/glfw"
	include "vendor/imgui"
	include "vendor/yaml-cpp"
	include "vendor/assimp"
	include "vendor/assimp/contrib/zlib"
group ""

project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "enginepch.h"
	pchsource "Engine/src/enginepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/yaml-cpp/include/**.h",
		"vendor/assimp/include/**.h",
		"vendor/assimp/include/**.hpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"vendor/spdlog/include",
		"%{Includedir.glfw}",
		"%{Includedir.dawn}",
		"%{Includedir.glm}",
		"%{Includedir.stb_image}",
		"%{Includedir.entt}",
		"%{Includedir.yaml_cpp}",
		"%{Includedir.assimp}"
	}


	links
	{
		"glfw",
		"webgpu_dawn",
		"yaml-cpp",
		"assimp",
	}

	libdirs 
	{
		"vendor/dawn"
	}

	-- filter "files:Engine/vendor/ImGuizmo/**.cpp"
	-- 	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/MP" }  -- Enable multithreading for Visual Studio

		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"ENGINE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",

    "vendor/imgui/backends/imgui_impl_wgpu.cpp",
		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"vendor",

		"Engine/src",
		"vendor/spdlog/include",

		"%{Includedir.glfw}",
		"%{Includedir.glm}",
		"%{Includedir.entt}",
		"%{Includedir.ImGui}",
		"%{Includedir.ImGuizmo}",
		"%{Includedir.assimp}",
		"%{Includedir.dawn}"       -- For ImGui WebGPU backend
	}

	links
	{
		"Engine",
		"ImGui",
	}

	postbuildcommands 
	{
    -- Copy webgpu_dawn.dll next to the built .exe
    '{COPY} "%{wks.location}/vendor/dawn/webgpu_dawn.dll" "%{cfg.targetdir}"'
	}


	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"
		buildoptions { "/MP" }
		defines
		{
			"ENGINE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"