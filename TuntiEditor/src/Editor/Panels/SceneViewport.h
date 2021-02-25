#pragma once

class Tunti::Scene;

namespace TEditor
{
	namespace SceneViewport
	{
		void OnStart();
		void OnRender();
		void OnPlay();

		Tunti::Scene& GetSceneContext();
	};
}