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

	filter "system:linux"
		pic "on"
		systemversion "latest"
		

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"


