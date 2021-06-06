#pragma once
#include "ScriptableRenderPipeline.h"

namespace Tunti
{
	class Renderer
	{
	public:
		static void Init();

		static void Destroy();

		static void OutputToScreenFramebuffer(Texture2D texture);

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void SetRenderPipeline(ScriptableRenderPipeline* renderPipeline);

		static ScriptableRenderPipeline* GetRenderPipeline() { return s_RenderPipeline; }

	private:

		virtual void OutputToScreenFramebufferImpl(Texture2D texture) const = 0;

	private:

		static inline Renderer* s_Renderer = nullptr;

		static inline ScriptableRenderPipeline* s_RenderPipeline = nullptr;

		static inline std::once_flag s_Context;
	};
}