#pragma once
#include "Editor/EditorAPI.h"

#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

namespace TEditor
{
	class OpenGLEditor : public EditorAPI
	{
	public:
		OpenGLEditor();
		~OpenGLEditor();
	};
}