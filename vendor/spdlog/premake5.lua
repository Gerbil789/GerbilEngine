project "spdlog"
kind "StaticLib"
language "C++"
staticruntime "on"

files
{
	"include/**.h",
	"src/**.cpp"
}

includedirs
{
  "include"
}

defines
{
	"SPDLOG_COMPILED_LIB"
}

filter "system:windows"
	systemversion "latest"
	buildoptions { "/MP" }
filter "configurations:Debug"
	runtime "Debug"
	symbols "on"
filter "configurations:Release"
	runtime "Release"
	optimize "on"