project "glslang"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    {
        "glslang/**.cpp",
        "SPIRV/**.cpp",

    }

    includedirs 
    { 
        ".",
        "glslang/Include",
        "SPIRV",
    }

