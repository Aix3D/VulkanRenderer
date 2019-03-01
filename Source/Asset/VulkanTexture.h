#pragma once
#include <gli\texture2d.hpp>
#include "..\RHI\VulkanRHI\VulkanDevice.h"

namespace Core
{
	class VulkanTexture
	{
	private:
		uint32 width;
		uint32 height;
		uint32 mipLevels;
		VkImage image;
		VkDeviceMemory deviceMemory;
		VkImageLayout imageLayout;
		VkSampler sampler;
		VkImageView view;
		
	public:
		gli::texture2d rawTexture;
		VkDescriptorImageInfo descriptor;
		void UploadToGPU(VulkanDevice * pDevice);
	};
}