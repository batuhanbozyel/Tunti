workspace "Tunti"
	architecture "x86_64"
	startproject "TuntiEditor"

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Doge/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Doge/vendor/Glad/include"
IncludeDir["spdlog"] = "%{wks.location}/Doge/vendor/spdlog/include"
IncludeDir["assimp"] = "%{wks.location}/Doge/vendor/assimp/include"
IncludeDir["glm"] = "%{wks.location}/Doge/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Doge/vendor/stb_image"
IncludeDir["ImGui"] = "%{wks.location}/TuntiEditor/vendor/imgui"
IncludeDir["entt"] = "%{wks.location}/Doge/vendor/entt/include"

LibraryDir = {}
LibraryDir["assimp"] = "%{wks.location}/Doge/vendor/assimp/lib"

group "Dependencies"
	include "vendor/premake"
	include "Doge/vendor/GLFW"
	include "Doge/vendor/Glad"
	include "TuntiEditor/vendor/imgui"
group ""

include "Doge"
include "TuntiEditor"
include "Sandbox"