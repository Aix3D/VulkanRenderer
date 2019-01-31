#include "VulkanCommandBuffer.h"

namespace Core
{
	VulkanCommandBuffer::VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBufferLevel level)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = level;
		commandBufferAllocateInfo.commandBufferCount = 1;
		VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &m_handle));
	}

	VkCommandBuffer VulkanCommandBuffer::GetHandle() const
	{
		return m_handle;
	}
}