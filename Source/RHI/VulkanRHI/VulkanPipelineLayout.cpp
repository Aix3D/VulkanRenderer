#include "VulkanPipelineLayout.h"

namespace Core
{
	void VulkanPipelineLayout::PushLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlag, uint32 binding)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.descriptorType = type;
		layoutBinding.stageFlags = stageFlag;
		layoutBinding.binding = binding;
		layoutBinding.descriptorCount = 1;

		m_layoutBindings.push_back(layoutBinding);
	}

	void VulkanPipelineLayout::Build(VkDevice device)
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pBindings = m_layoutBindings.data();
		descriptorSetLayoutCreateInfo.bindingCount = m_layoutBindings.size();
		
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout));

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;

		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
	}

	VkDescriptorSetLayout VulkanPipelineLayout::GetDescriptorSetLayoutHandle()
	{
		return m_descriptorSetLayout;
	}

	VkPipelineLayout VulkanPipelineLayout::GetPipelineLayoutHandle()
	{
		return m_pipelineLayout;
	}
}