project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	staticruntime "on"
	externalwarnings "Off"
	
	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/Engine/src",
		"%{wks.location}/vendor/dawn/include" 
	}

	externalincludedirs
	{
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/glm",
		"%{wks.location}/vendor/entt/include",
		"%{wks.location}/vendor/imgui",
		"%{wks.location}/vendor/ImGuizmo"
	}

	links
	{
		"Engine",
		"ImGui",
	}

	postbuildcommands 
	{
		"{COPY} %{wks.location}/vendor/dawn/webgpu_dawn.dll %{cfg.targetdir}",
		"{COPY} %{wks.location}/Resources %{cfg.targetdir}/Resources"
	}

	postbuildmessage "Copying dependencies..."

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