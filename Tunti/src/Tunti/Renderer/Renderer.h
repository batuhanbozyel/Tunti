#pragma once

#include "tpch.h"
#include "glm/glm.hpp"

namespace Tunti
{
	enum class RendererAPI
	{
		None = 0, OpenGL = 1
	};

	class RendererCommand
	{
	public:
		virtual ~RendererCommand() = default;

		virtual void ClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
	};

	class Renderer
	{
	public:
		static void Init();

		static void ClearColor(const glm::vec4& color);
		static void Clear();

		inline static RendererAPI CurrentAPI() { return s_RendererAPI; }
	private:
		static RendererAPI s_RendererAPI;
		static RendererCommand* s_RendererCommand;
	};
}