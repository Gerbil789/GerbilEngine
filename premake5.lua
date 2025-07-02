workspace "GerbilEngine"
	architecture "x64"
	startproject "Editor"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

Includedir = {}
-- Engine dependencies
Includedir["glfw"] = "Engine/vendor/glfw/include" 				-- Windowing library
Includedir["dawn"] = "Engine/vendor/dawn/include" 				-- WebGPU implementation
Includedir["glm"] = "Engine/vendor/glm" 									-- Math library
Includedir["stb_image"] = "Engine/vendor/stb_image" 			-- Image loading library
Includedir["entt"] = "Engine/vendor/entt/include" 				-- ECS library
Includedir["yaml_cpp"] = "Engine/vendor/yaml-cpp/include" -- YAML parsing library
Includedir["assimp"] = "Engine/vendor/assimp/include" 		-- 3D model loading library

-- Editor dependencies
Includedir["ImGui"] = "Editor/vendor/imgui"
Includedir["ImGuizmo"] = "Editor/vendor/ImGuizmo"


group "Dependencies"
	include "Engine/vendor/glfw"
	include "Editor/vendor/imgui"
	include "Engine/vendor/yaml-cpp"
	include "Engine/vendor/assimp"
	include "Engine/vendor/assimp/contrib/zlib"
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
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/yaml-cpp/include/**.h",
		"%{prj.name}/vendor/assimp/include/**.h",
		"%{prj.name}/vendor/assimp/include/**.hpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
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
		"Engine/vendor/dawn"
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
    "%{prj.name}/vendor/imgui/backends/imgui_impl_wgpu.cpp",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
	}

	includedirs
	{
		"Editor/src",
		"Editor/vendor",

		"Engine/src",
		"Engine/vendor",
		"Engine/vendor/spdlog/include",

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
    '{COPY} "../Engine/vendor/dawn/webgpu_dawn.dll" "%{cfg.targetdir}"'
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