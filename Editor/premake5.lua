project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	files
	{
		"src/**.h",
		"src/**.cpp"
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