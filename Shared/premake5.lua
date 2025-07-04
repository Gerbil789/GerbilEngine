project "Shared"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/vendor/imgui"
	}

	links
	{
		"ImGui"
	}

	filter "system:windows"
		systemversion "latest"
		defines { "ENGINE_PLATFORM_WINDOWS" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines { "RELEASE" }
		optimize "on"
		runtime "Release"