#pragma once
#include "Doge/Renderer/ShaderDataType.h"
#include <glad/glad.h>

namespace Doge
{
	static uint32_t ConvertToOpenGLBaseType(const ShaderDataType::Type type)
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
}