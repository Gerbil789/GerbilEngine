project "glaze"
kind "None"
systemversion "latest"

files
{
  "include/**.hpp",
  "include/**.h",
}

includedirs
{
  "include",
}

filter "configurations:Debug"
  runtime "Debug"
  symbols "on"

filter "configurations:Release"
  runtime "Release"
  optimize "on"