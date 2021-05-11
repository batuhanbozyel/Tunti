workspace "Tunti"
	architecture "x86_64"
	startproject "TuntiEditor"

	configurations
	{
		"Debug",
		"RelWithDebugInfo",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Tunti/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Tunti/vendor/Glad/include"
IncludeDir["spdlog"] = "%{wks.location}/Tunti/vendor/spdlog/include"
IncludeDir["glm"] = "%{wks.location}/Tunti/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Tunti/vendor/stb_image"
IncludeDir["ImGui"] = "%{wks.location}/TuntiEditor/vendor/imgui"
IncludeDir["entt"] = "%{wks.location}/Tunti/vendor/entt/include"
IncludeDir["tinyobjloader"] = "%{wks.location}/Tunti/vendor/tinyobjloader"

group "Dependencies"
	include "vendor/premake"
	include "Tunti/vendor/GLFW"
	include "Tunti/vendor/Glad"
	include "TuntiEditor/vendor/imgui"
group ""

include "Tunti"
include "TuntiEditor"
include "Sandbox" -- Test program that only exists in local