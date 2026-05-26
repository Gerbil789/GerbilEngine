project "miniaudio"
kind "StaticLib"
language "C"
staticruntime "on"
warnings "Off"
targetdir ("bin/" .. outputdir .. "/%{prj.name}")
objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

files
{
	"**.h",
	"**.c"
}

includedirs
{
	"%{wks.location}/vendor/miniaudio"
}