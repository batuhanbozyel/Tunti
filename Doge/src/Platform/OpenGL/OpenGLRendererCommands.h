#pragma once
#include "Doge/Renderer/RendererCommands.h"

namespace Doge
{
	class OpenGLRendererCommands : public RendererAPICommands
	{
	public:
		virtual void APIDrawIndexed(uint32_t count) const override;

		virtual void APISetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const override;
		virtual void APIClearColor(const glm::vec4& color) const override;
		virtual void APIClear() const override;
	};
}