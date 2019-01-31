#pragma once

//	智能指针
#include <memory>

//	断言
#include <assert.h>

//	平台相关定义
#include "../Platform/Platform.h"

//	定义好,方便日后更换STL库
#define ctd std

#define VECTOR_INCLUDE_PATH <vector>
#define ARRAY_INCLUDE_PATH <array>
#define LIST_INCLUDE_PATH <list>
#define STACK_INCLUDE_PATH <stack>
#define MAP_INCLUDE_PATH <map>
#define SET_INCLUDE_PATH <set>
#define HASH_MAP_INCLUDE_PATH <hash_map>
#define HASH_SET_INCLUDE_PATH <hash_set>
#define STRING_INCLUDE_PATH <string>
#define FUNCTIONAL_INCLUDE_PATH <functional>
#define ALGORITHM_INCLUDE_PATH <algorithm>

#define True 1
#define False 0
#define Null 0

const Core::int32 InvalidIndexInt32 = -1;
const Core::uint32 InvalidIndexUint32 = -1;
const Core::uint32 MaxUint32 = 1 << 31;

namespace Core
{
	//	错误代码,无错误.
	extern ErrorCode ErrorCode_OK;

	struct InputState
	{
		Bool inlvaid = False;

		//	KeyboardState
		Bool wDown = False;
		Bool sDown = False;
		Bool aDown = False;
		Bool dDown = False;
		Bool qDown = False;
		Bool eDown = False;
		Bool leftShiftDown = False;
		
		//	MouseState
		Bool leftBuffonDown = False;
		Bool middleBuffonDown = False;
		Bool rightBuffonDown = False;
		uint32 xPos = 0;
		uint32 yPos = 0;
		uint32 zPos = 1;
	};

	struct WindowsState
	{
		int32 xPos = 0;
		int32 yPos = 0;
		int32 width = 0;
		int32 height = 0;
		ANSICHAR * titleName;
	};
}
/*
//	暂不使用这些.
#define STDCALL __stdcall
#define CDECL __cdecl  
#define FASTCALL __fastcall
*/