#pragma once

class Tunti::Entity;

namespace TEditor
{
	namespace SceneHierarchy
	{
		void OnRender();

		Tunti::Entity& GetSelectedEntity();
	};
}