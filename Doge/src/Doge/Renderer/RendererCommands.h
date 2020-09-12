#pragma once

namespace Doge
{
	enum class CullFace : unsigned char
	{
		Front, Back
	};

	class RendererCommands
	{
	public:
		static void Init();
		// Renderer Commands
		static void DrawIndexed(uint32_t count);

		static void DisableFaceCulling();
		static void EnableFaceCulling(const CullFace& face);

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void ClearColor(const glm::vec4& color);
		static void Clear();
	protected:
		virtual void DrawIndexedImpl(uint32_t count) const = 0;

		virtual void DisableFaceCullingImpl() const = 0;
		virtual void EnableFaceCullingImpl(const CullFace& face) const = 0;

		virtual void SetViewportImpl(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const = 0;
		virtual void ClearColorImpl(const glm::vec4& color) const = 0;
		virtual void ClearImpl() const = 0;
	private:
		static std::unique_ptr<RendererCommands> s_RendererAPI;
	};
}
