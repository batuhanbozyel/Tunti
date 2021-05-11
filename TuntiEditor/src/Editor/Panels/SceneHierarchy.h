#pragma once

class Tunti::Entity;

namespace TEditor
{
	namespace SceneHierarchy
	{
		void OnImGuiRender();

		Tunti::Entity& GetSelectedEntity();
	};
}