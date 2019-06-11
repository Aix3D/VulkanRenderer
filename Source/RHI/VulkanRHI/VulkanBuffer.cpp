#include "VulkanBuffer.h"

namespace Core
{
	VulkanBuffer::VulkanBuffer()
		:mapped(Null)
	{

	}

	void VulkanBuffer::Map(VkDevice device, VkDeviceSize offset, VkDeviceSize size)
	{
		VK_CHECK_RESULT(vkMapMemory(device, memory, offset, size, 0, &mapped));
	}

	void VulkanBuffer::Unmap(VkDevice device, VkDeviceSize size)
	{
		if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange mappedRange{};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;

			mappedRange.memory = memory;
			mappedRange.offset = 0;
			mappedRange.size = size;
			vkFlushMappedMemoryRanges(device, 1, &mappedRange);
		}

		vkUnmapMemory(device, memory);

		VK_CHECK_RESULT(vkBindBufferMemory(device, buffer, memory, 0));
	}

	void VulkanBuffer::SetDescriptorBufferInfo()
	{
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = VK_WHOLE_SIZE;
		descriptorBufferInfo.buffer = buffer;
	}

	VulkanBuffer::~VulkanBuffer()
	{

	}
}