project "yaml-cpp"
kind "StaticLib"
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

filter "system:windows"
  systemversion "latest"
  buildoptions {"/permissive-", "/std:c++latest" }

filter "system:linux"
  pic "On"
  systemversion "latest"

filter "configurations:Debug"
  runtime "Debug"
  symbols "on"

filter "configurations:Release"
  runtime "Release"
  optimize "on"