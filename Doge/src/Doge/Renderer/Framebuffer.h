#pragma once

namespace Doge
{
	constexpr uint32_t MaxFramebufferSize = 8192;

	struct FramebufferSpecification
	{
		FramebufferSpecification() = default;

		FramebufferSpecification(uint32_t width, uint32_t height, uint32_t samples = 1, bool swapChain = false)
			: Width(width), Height(height), Samples(samples), SwapChainTarget(swapChain) {}

		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};
}
