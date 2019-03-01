#include "VulkanDescriptorSet.h"

using namespace ctd;

namespace Core
{
	void VulkanDescriptorSet::SetDescriptorSetLayout(VkDescriptorSetLayout layout)
	{
		m_layout = layout;
	}

	void VulkanDescriptorSet::PushDescriptorInfo(VkDescriptorBufferInfo bufferInfo)
	{
		m_bufferInfos.push_back(bufferInfo);
	}

	void VulkanDescriptorSet::PushDescriptorInfo(VkDescriptorImageInfo imageInfo)
	{
		m_imageInfos.push_back(imageInfo);
	}

	void VulkanDescriptorSet::Build(VkDevice device, VkDescriptorPool descriptorPool)
	{
		//	Creating descriptor sets
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = &m_layout;
		descriptorSetAllocateInfo.descriptorSetCount = 1;

		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &m_descriptorSet));

		//	Updating descriptor sets
		vector<VkWriteDescriptorSet> writeDescriptorSets;

		int32 binding = 0;

		for (int32 i = 0; i < m_bufferInfos.size(); ++i)
		{
			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = m_descriptorSet;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescriptorSet.dstBinding = binding++;
			writeDescriptorSet.pBufferInfo = &m_bufferInfos[i];
			writeDescriptorSet.descriptorCount = 1;

			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		for (int32 i = 0; i < m_imageInfos.size(); ++i)
		{
			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = m_descriptorSet;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet.dstBinding = binding++;
			writeDescriptorSet.pImageInfo = &m_imageInfos[i];
			writeDescriptorSet.descriptorCount = 1;

			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
	}

	VkDescriptorSet VulkanDescriptorSet::GetHandle() const
	{
		return m_descriptorSet;
	}
}