#pragma once
#include <BaseInclude.h>

namespace Core
{
	class RawShader
	{
	public:
		std::unique_ptr<ANSICHAR *> pRawData;
		size_t size;
	};
}