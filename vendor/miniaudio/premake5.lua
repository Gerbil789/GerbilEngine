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