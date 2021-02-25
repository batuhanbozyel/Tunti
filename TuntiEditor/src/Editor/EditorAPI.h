#pragma once

namespace TEditor
{
	class EditorLayer;

	class EditorAPI
	{
	public:
		virtual ~EditorAPI() = default;
	protected:
		std::function<void()> BeginRender;
		std::function<void()> EndRender;

		friend class EditorLayer;
	};
}