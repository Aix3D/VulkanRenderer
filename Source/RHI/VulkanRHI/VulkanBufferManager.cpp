#include "VulkanBufferManager.h"

namespace Core
{
	Core::uint32 VulkanBufferManager::getMemoryType(uint32 typeBits, VkMemoryPropertyFlags properties)
	{
		for (uint32 i = 0; i < m_deviceMemoryPropertiesRef.memoryTypeCount; ++i)
		{
			if ((typeBits & 1) == 1)
			{
				if ((m_deviceMemoryPropertiesRef.memoryTypes[i].propertyFlags & properties) == properties)
				{

					return i;
				}
			}

			typeBits >>= 1;
		}

		assert(False);

		return 0;
	}

	VulkanBufferManager::VulkanBufferManager(VkDevice device, VkPhysicalDeviceMemoryProperties deviceMemoryProperties)
	{
		m_deviceRef = device;
		m_deviceMemoryPropertiesRef = deviceMemoryProperties;
	}

	std::shared_ptr<Core::VulkanBufferP> VulkanBufferManager::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size)
	{
		std::shared_ptr<VulkanBufferP> pBuffer = std::make_shared<VulkanBufferP>();

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.usage = usageFlags;
		bufferCreateInfo.size = size;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK_RESULT(vkCreateBuffer(m_deviceRef, &bufferCreateInfo, nullptr, &pBuffer->buffer));

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(m_deviceRef, pBuffer->buffer, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = getMemoryType(memoryRequirements.memoryTypeBits, memoryPropertyFlags);
		VK_CHECK_RESULT(vkAllocateMemory(m_deviceRef, &memoryAllocateInfo, nullptr, &pBuffer->memory));

		pBuffer->memoryPropertyFlags = memoryPropertyFlags;

		m_buffers.push_back(pBuffer);

		return pBuffer;
	}

	VulkanBufferManager::~VulkanBufferManager()
	{

	}
}