#pragma once
#include "Doge/Renderer/RendererCommands.h"

namespace Doge
{
	class OpenGLRendererCommands : public RendererCommands
	{
	public:
		virtual void DrawIndexedImpl(uint32_t count) const override;

		virtual void DisableFaceCullingImpl() const override;
		virtual void EnableFaceCullingImpl(const CullFace& face) const override;

		virtual void SetViewportImpl(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const override;
		virtual void ClearColorImpl(const glm::vec4& color) const override;
		virtual void ClearImpl() const override;
	};
}