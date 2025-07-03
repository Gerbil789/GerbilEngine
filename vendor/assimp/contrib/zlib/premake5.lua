project "zlib"
    kind "StaticLib"
    architecture "x64"
    language "C"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "**.c",
        "**.h"
    }

    includedirs {
        "",
        -- "contrib"
    }

    defines { "NO_ASM" }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"