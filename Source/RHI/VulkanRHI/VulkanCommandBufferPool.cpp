#include "VulkanCommandBufferPool.h"

namespace Core
{
	VulkanCommandBufferPool::VulkanCommandBufferPool()
	{
	}

	void VulkanCommandBufferPool::Create(VkDevice device, uint32 queueFamilyIndex)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_commandPool));
	}

	std::unique_ptr<Core::VulkanCommandBuffer> VulkanCommandBufferPool::CreateCommandBuffer(VkDevice device, VkCommandBufferLevel level)
	{
		VulkanCommandBuffer * pData = new VulkanCommandBuffer(device, m_commandPool, level);

		std::unique_ptr<VulkanCommandBuffer> vulkanCommandBuffer = std::make_unique<VulkanCommandBuffer>(pData);

		return vulkanCommandBuffer;
	}

	VulkanCommandBufferPool::~VulkanCommandBufferPool()
	{

	}

}