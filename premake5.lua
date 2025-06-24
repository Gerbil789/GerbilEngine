workspace "GerbilEngine"
	architecture "x64"
	startproject "Editor"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

Includedir = {}
Includedir["GLFW"] = "Engine/vendor/GLFW/include"
Includedir["Glad"] = "Engine/vendor/Glad/include"
Includedir["glm"] = "Engine/vendor/glm"
Includedir["stb_image"] = "Engine/vendor/stb_image"
Includedir["entt"] = "Engine/vendor/entt/include"
Includedir["yaml_cpp"] = "Engine/vendor/yaml-cpp/include"
Includedir["assimp"] = "Engine/vendor/assimp/include"
Includedir["SPIRV_Cross"] = "Engine/vendor/SPIRV-Cross/include"
Includedir["spirv_tools"] = "Engine/vendor/shaderc/third_party/spirv-tools/include"
Includedir["glslang"] = "Engine/vendor/shaderc/third_party/glslang/glslang/Include"
Includedir["shaderc"] = "Engine/vendor/shaderc/libshaderc/include"

Includedir["ImGui"] = "Editor/vendor/imgui"
Includedir["ImGuizmo"] = "Editor/vendor/ImGuizmo"


group "Dependencies"
	include "Engine/vendor/GLFW"
	include "Engine/vendor/Glad"
	include "Editor/vendor/imgui"
	include "Engine/vendor/yaml-cpp"
	include "Engine/vendor/assimp"
	include "Engine/vendor/assimp/contrib/zlib"
	include "Engine/vendor/shaderc/third_party/spirv-tools"
	include "Engine/vendor/shaderc/third_party/glslang"
	include "Engine/vendor/shaderc"
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
		-- "%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		-- "%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
		"%{prj.name}/vendor/assimp/include/**.h",
		"%{prj.name}/vendor/assimp/include/**.hpp",
		"%{prj.name}/vendor/SPIRV-Cross/include/**.h",
		"%{prj.name}/vendor/SPIRV-Cross/include/**.hpp",
		"%{prj.name}/vendor/SPIRV-Cross/include/**.c",
		"%{prj.name}/vendor/SPIRV-Cross/include/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{Includedir.GLFW}",
		"%{Includedir.Glad}",
		-- "%{Includedir.ImGui}",
		"%{Includedir.glm}",
		"%{Includedir.stb_image}",
		"%{Includedir.entt}",
		"%{Includedir.yaml_cpp}",
		-- "%{Includedir.ImGuizmo}",
		"%{Includedir.assimp}",
		"%{Includedir.spirv_tools}",
		"Engine/vendor/shaderc/third_party/spirv-tools/external/spirv-headers/include",
		"Engine/vendor/shaderc/third_party/spirv-tools/external/spirv-headers/include/spirv/unified1",
		"%{Includedir.glslang}",
		"%{Includedir.shaderc}",
		"%{Includedir.SPIRV_Cross}"

	}


	links
	{
		"GLFW",
		"Glad",
		-- "ImGui",
		"opengl32.lib",
		"yaml-cpp",
		"assimp",
		"spirv-tools",
		"shaderc",
	}

	-- filter "files:Engine/vendor/ImGuizmo/**.cpp"
	-- 	flags { "NoPCH" }

	filter { "files:Engine/vendor/SPIRV-Cross/include/**.cpp" }
    	flags { "NoPCH" }

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
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"Engine/vendor",
		"Editor/vendor",
		"%{Includedir.GLFW}",
		"%{Includedir.glm}",
		"%{Includedir.entt}",
		"%{Includedir.ImGui}",
		"%{Includedir.ImGuizmo}",
		"%{Includedir.assimp}",
		"%{Includedir.glslang}",
	}

	links
	{
		"Engine",
		"ImGui",
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