#include "Tunti.h"
#include "Utils.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Utils
{
	std::string OpenFileDialog()
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(Tunti::Application::GetWindow()->GetHandle());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;

		ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		std::string fileName;
		if (GetOpenFileNameA(&ofn))
			fileName = ofn.lpstrFile;

		return fileName;
	}
}