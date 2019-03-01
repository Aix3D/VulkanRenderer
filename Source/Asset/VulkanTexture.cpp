#include "VulkanTexture.h"
#include "..\RHI\VulkanRHI\VulkanGlobal.h"

namespace Core
{
	void VulkanTexture::UploadToGPU(VulkanDevice * pDevice)
	{
		width = static_cast<uint32>(rawTexture.extent().x);
		height = static_cast<uint32>(rawTexture.extent().y);
		mipLevels = static_cast<uint32>(rawTexture.levels());

		VkFormat format = VK_FORMAT_BC3_UNORM_BLOCK;

		VkFormatProperties formatProperties = pDevice->GetPhysicalDeviceFormatProperties(format);

		VkCommandBuffer copyCmd = pDevice->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkBool32 useStaging = VK_TRUE;

		if (useStaging)
		{
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingMemory;

			VkBufferCreateInfo bufferCreateInfo{};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = rawTexture.size();
			bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			vkCreateBuffer(pDevice->GetLogicalDevice(), &bufferCreateInfo, nullptr, &stagingBuffer);

			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(pDevice->GetLogicalDevice(), stagingBuffer, &memoryRequirements);

			VkMemoryAllocateInfo memoryAllocateInfo{};
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.allocationSize = memoryRequirements.size;
			// Get memory type index for a host visible buffer
			memoryAllocateInfo.memoryTypeIndex = VulkanGlobal::Instance()->GetMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			vkAllocateMemory(pDevice->GetLogicalDevice(), &memoryAllocateInfo, nullptr, &stagingMemory);
			vkBindBufferMemory(pDevice->GetLogicalDevice(), stagingBuffer, stagingMemory, 0);

			uint8_t *data;
			vkMapMemory(pDevice->GetLogicalDevice(), stagingMemory, 0, memoryRequirements.size, 0, (void **)&data);
			memcpy(data, rawTexture.data(), rawTexture.size());
			vkUnmapMemory(pDevice->GetLogicalDevice(), stagingMemory);

			ctd::vector<VkBufferImageCopy> bufferCopyRegions;
			uint32 offset = 0;

			for (uint32 i = 0; i < mipLevels; i++)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = i;
				bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(rawTexture[i].extent().x);
				bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(rawTexture[i].extent().y);
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;

				bufferCopyRegions.push_back(bufferCopyRegion);

				offset += static_cast<uint32_t>(rawTexture[i].size());
			}

			VkImageCreateInfo imageCreateInfo{};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.format = format;
			imageCreateInfo.mipLevels = mipLevels;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCreateInfo.extent = { width, height, 1 };
			imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;

			if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
				imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

			vkCreateImage(pDevice->GetLogicalDevice(), &imageCreateInfo, nullptr, &image);
			vkGetImageMemoryRequirements(pDevice->GetLogicalDevice(), image, &memoryRequirements);
			memoryAllocateInfo.allocationSize = memoryRequirements.size;
			memoryAllocateInfo.memoryTypeIndex = VulkanGlobal::Instance()->GetMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			vkAllocateMemory(pDevice->GetLogicalDevice(), &memoryAllocateInfo, nullptr, &deviceMemory);
			vkBindImageMemory(pDevice->GetLogicalDevice(), image, deviceMemory, 0);

			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = mipLevels;
			subresourceRange.layerCount = 1;

			pDevice->SetImageLayout(
				copyCmd,
				image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				subresourceRange);

			vkCmdCopyBufferToImage(
				copyCmd,
				stagingBuffer,
				image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				static_cast<uint32_t>(bufferCopyRegions.size()),
				bufferCopyRegions.data()
			);

			imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			pDevice->SetImageLayout(
				copyCmd,
				image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				imageLayout,
				subresourceRange);

			pDevice->FlushCommandBuffer(copyCmd, true);

			vkFreeMemory(pDevice->GetLogicalDevice(), stagingMemory, nullptr);
			vkDestroyBuffer(pDevice->GetLogicalDevice(), stagingBuffer, nullptr);
		}
		else
		{
			//	TODO:
		}

		// Create a defaultsampler
		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerCreateInfo.minLod = 0.0f;
		// Max level-of-detail should match mip level count
		samplerCreateInfo.maxLod = (useStaging) ? (float)mipLevels : 0.0f;
		// Enable anisotropic filtering
		samplerCreateInfo.maxAnisotropy = 1;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		vkCreateSampler(pDevice->GetLogicalDevice(), &samplerCreateInfo, nullptr, &sampler);

		VkImageViewCreateInfo viewCreateInfo = {};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = format;
		viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		// Linear tiling usually won't support mip maps
		// Only set mip map count if optimal tiling is used
		viewCreateInfo.subresourceRange.levelCount = (useStaging) ? mipLevels : 1;
		viewCreateInfo.image = image;
		vkCreateImageView(pDevice->GetLogicalDevice(), &viewCreateInfo, nullptr, &view);

		descriptor.sampler = sampler;
		descriptor.imageView = view;
		descriptor.imageLayout = imageLayout;
	}
}