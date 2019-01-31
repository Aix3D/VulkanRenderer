#pragma once
#include <BaseInclude.h>
#include "VulkanBase.h"

namespace Core
{
	class VulkanCommandBuffer
	{
	private:
		VkCommandBuffer m_handle;
	public:
		VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBufferLevel level);
		VkCommandBuffer GetHandle()const;
	};
}