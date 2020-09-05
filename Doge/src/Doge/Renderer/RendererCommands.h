#pragma once

namespace Doge
{
	class RendererAPICommands
	{
	public:
		virtual void APIDrawIndexed(uint32_t count) const = 0;

		virtual void APISetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const = 0;
		virtual void APIClearColor(const glm::vec4& color) const = 0;
		virtual void APIClear() const = 0;
	};

	class RendererCommands
	{
	public:
		static void Init();
		// Renderer Commands
		static void DrawIndexed(uint32_t count);

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void ClearColor(const glm::vec4& color);
		static void Clear();
	private:
		static std::shared_ptr<RendererAPICommands> s_RendererAPI;
	};
}
