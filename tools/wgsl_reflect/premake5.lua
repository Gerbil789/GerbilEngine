project "wgsl_reflect"
kind "ConsoleApp"

files
{
	"**.h",
	"**.cpp",
}

includedirs
{
	"src",
}

externalincludedirs
{
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/glaze/include",
}

defines 
{ 
	"GLM_ENABLE_EXPERIMENTAL",
}