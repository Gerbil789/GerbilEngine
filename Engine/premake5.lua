project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "enginepch.h"
	pchsource "src/enginepch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"%{wks.location}/vendor/glm/glm/**.inl",
		"%{wks.location}/vendor/yaml-cpp/include/**.h",
		"%{wks.location}/vendor/tinygltf/**.h",
		"%{wks.location}/vendor/tinygltf/**.hpp",
	}

	includedirs
	{
		"src",
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/dawn/include",
		"%{wks.location}/vendor/glm",
		"%{wks.location}/vendor/entt/include",
		"%{wks.location}/vendor/yaml-cpp/include",
		"%{wks.location}/vendor/tinygltf",
		"%{wks.location}/vendor/portable-file-dialogs"
	}

	links
	{
		"glfw",
		"webgpu_dawn",
		"yaml-cpp"
	}

	libdirs 
	{
		"%{wks.location}/vendor/dawn"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/MP" } -- Enable multithreading for Visual Studio
		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"ENGINE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "system:linux"
    systemversion "latest"
    defines {
        "ENGINE_PLATFORM_LINUX",
        "GLFW_INCLUDE_NONE"
    }
    links {
        "pthread", "dl", "m", "X11", "Xrandr", "Xi", "Xcursor", "Xinerama", "GL", "udev"
    }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines { "RELEASE" }
		optimize "on"
		runtime "Release"