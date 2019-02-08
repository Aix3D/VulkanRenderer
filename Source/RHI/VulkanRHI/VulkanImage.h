#pragma once
#include "VulkanBase.h"

namespace Core
{
	class VulkanImage
	{
	private:
		VkImage m_image;
		VkDeviceMemory m_deviceMemory;

	public:
		VulkanImage(VkDevice logicalDevice, VkFormat format, uint32 width, uint32 height, uint32 depth, VkImageUsageFlags usage);
		VkImage GetHandle();
		~VulkanImage();
	};
}