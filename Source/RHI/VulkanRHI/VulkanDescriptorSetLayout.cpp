#include "VulkanDescriptorSetLayout.h"

namespace Core
{
	void VulkanDescriptorSetLayout::PushLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlag, uint32 binding, uint32 descriptorCount)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.descriptorType = type;
		layoutBinding.stageFlags = stageFlag;
		layoutBinding.binding = binding;
		layoutBinding.descriptorCount = descriptorCount;

		m_layoutBindings.push_back(layoutBinding);
	}

	void VulkanDescriptorSetLayout::Build(VkDevice device)
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pBindings = m_layoutBindings.data();
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(m_layoutBindings.size());

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout));
	}

	VkDescriptorSetLayout VulkanDescriptorSetLayout::GetHandle() const
	{
		return m_descriptorSetLayout;
	}
}