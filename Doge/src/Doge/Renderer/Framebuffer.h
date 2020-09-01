#pragma once

namespace Doge
{
	struct FramebufferSpecification
	{
		FramebufferSpecification(uint32_t width, uint32_t height)
			: Width(width), Height(height) {}

		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& specification);
		~Framebuffer();

		void Resize(uint32_t width, uint32_t height);

		void Bind() const;
		void Unbind() const;

		uint32_t GetColorAttachment() const { return m_ColorAttachment; }
		uint32_t GetViewportWidth() const { return m_Specification.Width; }
		uint32_t GetViewportHeight() const { return m_Specification.Height; }
	private:
		void Construct();
	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;
		FramebufferSpecification m_Specification;
	};
}
