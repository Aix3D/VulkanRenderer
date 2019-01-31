#pragma once
#include <BaseInclude.h>
#include "VulkanBase.h"
#include "VulkanBufferP.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanBufferManager
	{
	private:
		VkDevice m_deviceRef;
		VkPhysicalDeviceMemoryProperties m_deviceMemoryPropertiesRef;

		ctd::vector<std::shared_ptr<VulkanBufferP>> m_buffers;

		uint32 getMemoryType(uint32 typeBits, VkMemoryPropertyFlags properties);
	
	public:
		VulkanBufferManager(VkDevice device, VkPhysicalDeviceMemoryProperties deviceMemoryProperties);
		std::shared_ptr<VulkanBufferP> CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size);
		~VulkanBufferManager();
	};
}