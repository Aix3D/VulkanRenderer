#pragma once
#include <BaseInclude.h>
#include "VulkanBase.h"
#include "VulkanCommandBuffer.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanCommandBufferPool
	{
	private:
		VkCommandPool m_commandPool;
		//ctd::vector<std::unique_ptr<VulkanCommandBuffer>> m_commandBuffers;

	public:
		VulkanCommandBufferPool();
		void Create(VkDevice device, uint32 queueFamilyIndex);
		std::unique_ptr<VulkanCommandBuffer> CreateCommandBuffer(VkDevice device, VkCommandBufferLevel level);
		~VulkanCommandBufferPool();
	};
}