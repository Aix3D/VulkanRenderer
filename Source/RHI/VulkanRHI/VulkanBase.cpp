#include "VulkanBase.h"

namespace Core
{
	void vk_check_result(VkResult result)
	{
		assert(result == VK_SUCCESS);
	}
}