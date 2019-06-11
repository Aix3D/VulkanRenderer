#include "VulkanDescriptorPool.h"

namespace Core
{
	void VulkanDescriptorPool::PushPoolSize(VkDescriptorType type, uint32 descriptorCount)
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = type;
		poolSize.descriptorCount = descriptorCount;

		m_poolSizes.push_back(poolSize);
	}

	void VulkanDescriptorPool::Build(VkDevice device)
	{
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(m_poolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = m_poolSizes.data();
		descriptorPoolCreateInfo.maxSets = static_cast<uint32>(m_poolSizes.size());	//	maxSets defines the maximum allowed DescriptorSets that this pool is allowed to make : http://web.engr.oregonstate.edu/~mjb/cs519v/StudentDiscussions/Hammock_InDepthOverviewOfDescriptors.4pp.pdf

		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool));
	}

	VkDescriptorPool VulkanDescriptorPool::GetHandle() const
	{
		return m_descriptorPool;
	}
}