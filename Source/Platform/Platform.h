#pragma once
#include "Windows/WindowsPlatform.h"

namespace Core
{
//	TOOD:	将来可以改为在编译器内定义?
#define PLATFORM_WINDOWS

#if !defined(PLATFORM_WINDOWS)
	#define PLATFORM_WINDOWS 0
#endif

#ifdef PLATFORM_WINDOWS
	typedef WindowsPlatformTypes PlatformTypes;
#endif

#if defined(PLATFORM_WINDOWS)
#include "Windows/WindowsPlatformCompilerPreSetup.h"
#endif

	typedef PlatformTypes::uint8		uint8;
	typedef PlatformTypes::uint16		uint16;
	typedef PlatformTypes::uint32		uint32;
	typedef PlatformTypes::uint64		uint64;

	typedef	PlatformTypes::int8			int8;
	typedef PlatformTypes::int16		int16;
	typedef PlatformTypes::int32		int32;
	typedef PlatformTypes::int64		int64;

	typedef PlatformTypes::ANSICHAR	ANSICHAR;
	typedef PlatformTypes::WIDECHAR	WIDECHAR;
	typedef PlatformTypes::TCHAR		TCHAR;
	typedef PlatformTypes::CHAR8		UTF8CHAR;
	typedef PlatformTypes::CHAR16		UCS2CHAR;
	typedef PlatformTypes::CHAR16		UTF16CHAR;
	typedef PlatformTypes::CHAR32		UTF32CHAR;

	typedef PlatformTypes::SIZE_T Size_T;
	typedef PlatformTypes::SSIZE_T SSize_T;

	typedef PlatformTypes::Bool		Bool;

	typedef PlatformTypes::TYPE_OF_NULL	TYPE_OF_NULL;
	typedef PlatformTypes::TYPE_OF_NULLPTR	TYPE_OF_NULLPTR;

	typedef PlatformTypes::ErrorCore ErrorCode;
}