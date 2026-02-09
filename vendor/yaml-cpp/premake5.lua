project "yaml-cpp"
kind "StaticLib"
language "C++"
cppdialect "C++23"
staticruntime "off"
warnings "Off"

files
{
  "include/**.h",
  "src/**.cpp"
}

includedirs
{
  "include"
}

defines { "YAML_CPP_STATIC_DEFINE" }
-- defines { "yaml_cpp_EXPORTS" }

filter "system:windows"
  systemversion "latest"

filter "configurations:Debug"
  runtime "Debug"
  symbols "on"

filter "configurations:Release"
  runtime "Release"
  optimize "on"