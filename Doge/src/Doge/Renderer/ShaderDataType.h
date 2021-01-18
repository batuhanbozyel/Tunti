#pragma once

namespace Doge
{
	class ShaderDataType
	{
	public:
		enum {
			Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, UInt, UInt2, UInt3, UInt4, Bool
		};

		static constexpr uint32_t Size(decltype(ShaderDataType::Float) type)
		{
			switch (type)
			{
				case Float:    return 4;
				case Float2:   return 4 * 2;
				case Float3:   return 4 * 3;
				case Float4:   return 4 * 4;
				case Mat3:     return 4 * 3 * 3;
				case Mat4:     return 4 * 4 * 4;
				case Int:      return 4;
				case Int2:     return 4 * 2;
				case Int3:     return 4 * 3;
				case Int4:     return 4 * 4;
				case UInt:     return 4;
				case UInt2:    return 4 * 2;
				case UInt3:    return 4 * 3;
				case UInt4:    return 4 * 4;
				case Bool:     return 1;
			}

			LOG_ASSERT(false, "Shader Type Size: Unknown Shader data type!");
			return 0;
		}
	};
}