project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
  staticruntime "off"

	files
	{
		"**.h",
		"**.cpp",
		"%{wks.location}/vendor/ImGuizmo/ImGuizmo.cpp"
	}

	includedirs
	{
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/imgui",
		"%{wks.location}/vendor/dawn/include",
		"%{wks.location}/vendor/ImGuizmo"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/permissive-", "/std:c++latest", "-Wno-missing-braces"}
	filter "system:linux"
		pic "on"
		systemversion "latest"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

filter { "platforms:Windows" }
	defines
	{
		"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
	}

filter { "platforms:Web" }
  removefiles { "**.*" } -- Strip out all ImGui source code
  files { "dummy.cpp" }  -- Feed the linker the empty file