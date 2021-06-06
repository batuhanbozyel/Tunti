#pragma once

namespace Tunti
{
	struct DrawArraysIndirectParams
	{
		GLuint Count;
		GLuint PrimCount;
		GLuint First;
		GLuint BaseInstance;

		DrawArraysIndirectParams(GLuint count, GLuint primCount, GLuint first, GLuint baseInstance)
			: Count(count), PrimCount(primCount), First(first), BaseInstance(baseInstance)
		{

		}
	};

	struct DrawElementsIndirectParams
	{
		GLuint Count;
		GLuint PrimCount;
		GLuint FirstIndex;
		GLuint BaseVertex;
		GLuint BaseInstance;

		DrawElementsIndirectParams(GLuint count, GLuint primCount, GLuint firstIndex, GLuint baseVertex, GLuint baseInstance)
			: Count(count), PrimCount(primCount), FirstIndex(firstIndex), BaseVertex(baseVertex), BaseInstance(baseInstance)
		{

		}
	};

	const DrawArraysIndirectParams QuadIndirectParams = DrawArraysIndirectParams(3, 1, 0, 0);
	const DrawArraysIndirectParams CubeIndirectParams = DrawArraysIndirectParams(14, 1, 0, 0);
}