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

		uint32_t GetSceneTexture() const { return m_SceneFramebuffer->GetColorAttachment(); }
		uint32_t GetWidth() const { return m_SceneFramebuffer->GetViewportWidth(); }
		uint32_t GetHeight() const { return m_SceneFramebuffer->GetViewportHeight(); }
	private:
		std::unique_ptr<Doge::Framebuffer> m_SceneFramebuffer;
		std::vector<Doge::RenderData> m_RenderDatas;
	};
}
