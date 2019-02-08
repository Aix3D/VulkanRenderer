#pragma once
#include "VulkanBase.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanCommandPool
	{
	private:
		VkCommandPool m_commandPool;
		ctd::vector<VkCommandBuffer> m_commandBuffers;

	public:
		void Create(VkDevice logicalDevice, uint32 queueFamilyIndex);
		void CreateCommandBuffers(VkDevice logicalDevice, uint32 bufferCount);
	};
}