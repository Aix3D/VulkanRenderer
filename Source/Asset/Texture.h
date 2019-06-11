#pragma once
#include "IAsset.h"
#include <gli\texture2d.hpp>
#include "..\RHI\VulkanRHI\VulkanDevice.h"

namespace Core
{
	class Texture : public IAsset
	{
	private:
		VkImage image;
		VkDeviceMemory deviceMemory;
		VkImageLayout imageLayout;
		VkSampler sampler;
		VkImageView view;
		Bool m_uploadedToGPU;
		
	public:
		std::unique_ptr<uint8[]> pImage;
		uint32 imageDataSize;
		uint32 width;
		uint32 height;
		uint32 mipLevels;
		VkDescriptorImageInfo descriptor;

		Texture();
		void UploadToGPU(VulkanDevice * pDevice);
		virtual void OnPause() override;
		virtual void OnResume() override;
		virtual ~Texture();
	};
}