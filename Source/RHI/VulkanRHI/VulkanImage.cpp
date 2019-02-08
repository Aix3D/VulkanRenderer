#include "VulkanImage.h"
#include "VulkanGlobal.h"

namespace Core
{

	VulkanImage::VulkanImage(VkDevice logicalDevice, VkFormat format, uint32 width, uint32 height, uint32 depth, VkImageUsageFlags usage)
	{
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = NULL;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.extent = { width, height, depth };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = usage;
		imageCreateInfo.flags = 0;

		VK_CHECK_RESULT(vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &m_image));
	
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(logicalDevice, m_image, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = NULL;
		memoryAllocateInfo.allocationSize = 0;
		memoryAllocateInfo.memoryTypeIndex = 0;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = VulkanGlobal::Instance()->GetMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(logicalDevice, &memoryAllocateInfo, nullptr, &m_deviceMemory));
		VK_CHECK_RESULT(vkBindImageMemory(logicalDevice, m_image, m_deviceMemory, 0));
	}

	VkImage VulkanImage::GetHandle()
	{
		return m_image;
	}

	VulkanImage::~VulkanImage()
	{

	}

}