#pragma once

#include "Tunti/Renderer/Renderer.h"

namespace Tunti
{
	class OpenGLRendererCommand : public RendererCommand
	{
	public:
		virtual void ClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
	private:
		OpenGLRendererCommand() = default;
		friend Renderer;
	};
}