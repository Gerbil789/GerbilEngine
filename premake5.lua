workspace "GerbilEngine"
	architecture "x64"
	startproject "Editor"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/glfw"
	include "vendor/imgui"
	include "vendor/yaml-cpp"
group ""

include "Engine"
include "Editor"