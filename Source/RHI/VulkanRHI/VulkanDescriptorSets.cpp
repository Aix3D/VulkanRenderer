#include "VulkanDescriptorSets.h"

using namespace ctd;

namespace Core
{
	void VulkanDescriptorSets::PushDescriptorSetLayout(VkDescriptorSetLayout layout, VkDescriptorBufferInfo bufferInfo)
	{
		m_layouts.push_back(layout);
		m_bufferInfos.push_back(bufferInfo);
	}

	void VulkanDescriptorSets::Build(VkDevice device)
	{
		//	Creating descriptor pool
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32>(m_layouts.size());

		vector<VkDescriptorPoolSize> poolSizes;
		poolSizes.push_back(poolSize);

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
		descriptorPoolCreateInfo.maxSets = 1;

		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool));

		//	Creating descriptor sets
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = m_layouts.data();
		descriptorSetAllocateInfo.descriptorSetCount = m_layouts.size();

		m_descriptorSets.resize(m_layouts.size());

		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, m_descriptorSets.data()));

		//	Updating descriptor sets
		vector<VkWriteDescriptorSet> writeDescriptorSets;

		for (int32 i = 0; i < m_descriptorSets.size(); ++i)
		{
			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = m_descriptorSets[i];
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescriptorSet.dstBinding = 0;
			writeDescriptorSet.pBufferInfo = &m_bufferInfos[i];
			writeDescriptorSet.descriptorCount = 1;

			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
	}

	VkDescriptorSet * VulkanDescriptorSets::GetHandle()
	{
		return m_descriptorSets.data();
	}

	Core::uint32 VulkanDescriptorSets::GetHandleCount()
	{
		return m_descriptorSets.size();
	}
}