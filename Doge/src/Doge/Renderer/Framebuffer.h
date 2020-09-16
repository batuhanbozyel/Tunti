#pragma once

namespace Doge
{
	struct FramebufferSpecification
	{
		FramebufferSpecification() = default;

		FramebufferSpecification(uint32_t width, uint32_t height, uint32_t samples = 1, bool swapChain = false)
			: Width(width), Height(height), Samples(samples), SwapChainTarget(swapChain) {}

		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		static Scope<Framebuffer> Create(const FramebufferSpecification& specification);

		void Resize(uint32_t width, uint32_t height);

		virtual void BlitMultisampled(uint32_t fboID) const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void BindColorAttachment(uint32_t slot) const = 0;

		uint32_t GetFramebufferID() const { return m_RendererID; }
		uint32_t GetColorAttachment() const { return m_ColorAttachment; }
		uint32_t GetViewportWidth() const { return m_Specification.Width; }
		uint32_t GetViewportHeight() const { return m_Specification.Height; }
	protected:
		virtual void Construct() = 0;
	protected:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;
		FramebufferSpecification m_Specification;
	};
}
