#pragma once
#include "VulkanBase.h"
#include "VulkanDescriptorPool.h"
#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanDescriptorSet
	{
	private:
		VkDescriptorSetLayout m_layout;
		ctd::vector<VkDescriptorBufferInfo> m_bufferInfos;
		ctd::vector<VkDescriptorImageInfo> m_imageInfos;
		VkDescriptorSet m_descriptorSet;

	public:
		void SetDescriptorSetLayout(VkDescriptorSetLayout layout);
		void PushDescriptorInfo(VkDescriptorBufferInfo bufferInfo);
		void PushDescriptorInfo(VkDescriptorImageInfo imageInfo);
		void Build(VkDevice device, VkDescriptorPool descriptorPool);
		VkDescriptorSet GetHandle() const;
	};
}