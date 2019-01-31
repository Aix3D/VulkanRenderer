#pragma once
#include <BaseInclude.h>

#if defined(PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

namespace Core
{
	void vk_check_result(VkResult result);

#define VK_CHECK_RESULT(f)			\
	{								\
		vk_check_result(VkResult(f));	\
	}		
}