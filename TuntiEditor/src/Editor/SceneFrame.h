#pragma once
#include "Doge.h"

namespace TEditor
{
	class SceneFrame
	{
	public:
		SceneFrame(uint32_t viewportWidth, uint32_t viewportHeight);
		~SceneFrame() = default;

		void OnUpdate(float dt);
		void Render();

		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		uint32_t GetSceneTexture() const { return Doge::Renderer::GetFramebuffer()->GetColorAttachment(); }
		uint32_t GetWidth() const { return Doge::Renderer::GetFramebuffer()->GetViewportWidth(); }
		uint32_t GetHeight() const { return Doge::Renderer::GetFramebuffer()->GetViewportHeight(); }
	private:
		std::vector<Doge::RenderData> m_RenderDatas;
		Doge::PerspectiveCameraController m_CameraController;
	};
}
