project "miniaudio"
	kind "StaticLib"
	language "C"
  staticruntime "on"
	warnings "Off"

	files
	{
		"**.h",
		"**.c"
	}

	includedirs
	{
		"%{wks.location}/vendor/miniaudio"
	}

	filter "system:windows"
		systemversion "latest"
		toolset "clang"
		buildoptions { "/MP", "/permissive-", "/std:c++latest" } -- MP = Enable multithreading for Visual Studio

	filter "system:linux"
		pic "on"
		systemversion "latest"
		

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"


