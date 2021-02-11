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
IncludeDir["assimp"] = "%{wks.location}/Tunti/vendor/assimp/include"
IncludeDir["glm"] = "%{wks.location}/Tunti/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Tunti/vendor/stb_image"
IncludeDir["ImGui"] = "%{wks.location}/TuntiEditor/vendor/imgui"
IncludeDir["entt"] = "%{wks.location}/Tunti/vendor/entt/include"

LibraryDir = {}
LibraryDir["assimp"] = "%{wks.location}/Tunti/vendor/assimp/lib"

group "Dependencies"
	include "vendor/premake"
	include "Tunti/vendor/GLFW"
	include "Tunti/vendor/Glad"
	include "TuntiEditor/vendor/imgui"
group ""

include "Tunti"
include "TuntiEditor"
if os.isdir("Sandbox") then include "Sandbox" end -- Test program that only exists in local