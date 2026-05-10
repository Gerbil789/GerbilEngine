project "TestProject"
kind "SharedLib"
language "C++"
cppdialect "C++23"
staticruntime "off"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"
-- fatalwarnings { "All" }

targetdir ("%{prj.location}/bin/%{cfg.system}/%{cfg.buildcfg}")
objdir ("%{prj.location}/bin-int/%{cfg.system}/%{cfg.buildcfg}")

files
{
	"src/TestProject/**.h",
	"src/TestProject/**.cpp"
}

includedirs
{
	"src",
	"%{wks.location}/Engine/include",

}

externalincludedirs
{
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
}

links
{
	"Engine",
}

libdirs 
{
	--"%{wks.location}/vendor/dawn"
}

postbuildcommands 
{
	"{COPYFILE} %{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/Engine/Engine.dll %{cfg.targetdir}"
}

filter "system:windows"
	systemversion "latest"
	buildoptions 
	{ 
		"/permissive-", 
		"/std:c++latest", 
		"-Wno-invalid-offsetof", 
	}
	defines
	{
		-- "_HAS_CXX23=1",
		"ENGINE_PLATFORM_WINDOWS",
		"GLFW_INCLUDE_NONE",
		"GAME_BUILD_DLL"
	}

filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE" }
	optimize "on"
	runtime "Release"