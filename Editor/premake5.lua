project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"%{wks.location}/vendor/imgui/backends/imgui_impl_wgpu.cpp",
		"%{wks.location}/vendor/imgui/backends/imgui_impl_glfw.cpp",
		"%{wks.location}/vendor/ImGuizmo/ImGuizmo.h",
		"%{wks.location}/vendor/ImGuizmo/ImGuizmo.cpp",
	}

	includedirs
	{
		"src",
		"%{wks.location}/Engine/src",
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/glm",
		"%{wks.location}/vendor/entt/include",
		"%{wks.location}/vendor/imgui",
		"%{wks.location}/vendor/ImGuizmo",
		"%{wks.location}/vendor/dawn/include"    
	}

	links
	{
		"Engine",
		"ImGui",
	}

	postbuildcommands 
	{
    '{COPY} "%{wks.location}/vendor/dawn/webgpu_dawn.dll" "%{cfg.targetdir}"'
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/MP" }
		defines { "ENGINE_PLATFORM_WINDOWS" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines { "RELEASE" }
		optimize "on"
		runtime "Release"