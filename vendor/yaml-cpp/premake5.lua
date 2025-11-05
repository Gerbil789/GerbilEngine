project "yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "on"
	warnings "Off"

	files
	{
		"include/**.h",
    "src/**.cpp"
	}

	externalincludedirs
  {
    "include"
  }

	defines { "YAML_CPP_STATIC_DEFINE" } -- important! to build as static lib

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"