project "shaderc"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    {
        "include/**.h",
        "libshaderc/**.cpp",
        "libshaderc/**.h",
        "libshaderc/**.cpp",
        "libshaderc/**.cc",
        "libshaderc/**.c",
        "libshaderc_util/**.cpp",
        "libshaderc_util/**.cc",

        "third_party/spirv-tools/source/**.cpp",
        "third_party/spirv-tools/generated/**.inc",

        "third_party/glslang/**.cpp",
    }

    includedirs 
    {
        "include",
        "libshaderc/include",
        "libshaderc_util/include",

        "third_party/spirv-headers/include",
        "third_party/spirv-tools/include",
        "third_party/spirv-tools/source",
        "third_party/spirv-tools",
        "third_party/spirv-tools/external/spirv-headers/include",
        "third_party/spirv-tools/external/spirv-headers/include/spirv/unified1",
        "third_party/spirv-tools/generated",

        "third_party/glslang",
    }

    links 
    { 
        "spirv-tools",
        "glslang" 
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


