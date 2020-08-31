#pragma once
#include <glad/glad.h>

#include "Doge/Core/Log.h"

namespace Doge
{
	enum class ShaderDataType
	{
		Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, UInt, UInt2, UInt3, UInt4, Bool,
	};

	static uint32_t ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::UInt:     return GL_UNSIGNED_INT;
		case ShaderDataType::UInt2:    return GL_UNSIGNED_INT;
		case ShaderDataType::UInt3:    return GL_UNSIGNED_INT;
		case ShaderDataType::UInt4:    return GL_UNSIGNED_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}

		LOG_ASSERT(false, "OpenGL Base Type: Unknown Shader data type!");
		return 0;
	}

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::UInt:     return 4;
		case ShaderDataType::UInt2:    return 4 * 2;
		case ShaderDataType::UInt3:    return 4 * 3;
		case ShaderDataType::UInt4:    return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		LOG_ASSERT(false, "Shader Type Size: Unknown Shader data type!");
		return 0;
	}
}