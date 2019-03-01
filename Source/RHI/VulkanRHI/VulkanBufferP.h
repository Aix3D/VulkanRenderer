#pragma once
#include <BaseInclude.h>
#include "VulkanBase.h"

namespace Core
{
	class VulkanBufferP
	{
	public:
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkMemoryPropertyFlags memoryPropertyFlags;
		VkDescriptorBufferInfo descriptorBufferInfo;
		void* mapped;

		VulkanBufferP();
		void Map(VkDevice device, VkDeviceSize offset, VkDeviceSize size);
		void Unmap(VkDevice device, VkDeviceSize size);
		void SetDescriptorBufferInfo();
		~VulkanBufferP();
	};
}