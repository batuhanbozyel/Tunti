#pragma once

namespace Doge
{
	class RendererCommands
	{
	public:
		// Render Commands
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void ClearColor(const glm::vec4& color);
		static void Clear();
	};
}
