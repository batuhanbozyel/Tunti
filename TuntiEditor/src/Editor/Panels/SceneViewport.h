#pragma once

class Tunti::Event;
class Tunti::Scene;

namespace TEditor
{
	namespace SceneViewport
	{
		void OnEvent(Tunti::Event& e);
		void OnStart();
		void OnRender();
		void OnEditorUpdate(double dt);
		void OnPlay(double dt);

		Tunti::Scene& GetSceneContext();
		std::pair<float, float> GetViewportSize();
	};
}