project "Launcher"
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
		"%{wks.location}/vendor/imgui/backends/imgui_impl_wgpu.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/vendor/imgui",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/imgui",
		"%{wks.location}/vendor/dawn/include"       
	}

	links
	{
		"ImGui",
		"webgpu_dawn",
		"Shared"
	}

	libdirs 
	{
		"%{wks.location}/vendor/dawn"
	}

	postbuildcommands 
	{
    -- Copy webgpu_dawn.dll next to the built .exe
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