#pragma once
#include "Tunti.h"

namespace TEditor
{
	class SceneFrame
	{
	public:
		SceneFrame(uint32_t viewportWidth, uint32_t viewportHeight);
		~SceneFrame() = default;

		void OnUpdate();
		void Render();

		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);
	};
}
