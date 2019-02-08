#pragma once
#include "VulkanBase.h"

namespace Core
{
	class VulkanImageView
	{
	private:
		VkImageView m_imageView;
	public:
		VulkanImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask, VkImage image);
		VkImageView GetHandle();
		~VulkanImageView();
	};
}