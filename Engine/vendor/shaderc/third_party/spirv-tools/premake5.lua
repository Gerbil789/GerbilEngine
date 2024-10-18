project "spirv-tools"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    {
        "source/**.h",
        "source/**.cpp",
        "generated/**.inc",
    }

    includedirs 
    {
        ".",   
        "include",
        "generated",
        "external/spirv-headers/include",
        "external/spirv-headers/include/spirv/unified1",
    }

    filter "system:windows"
        defines { "WIN32" }
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        runtime "Release"

