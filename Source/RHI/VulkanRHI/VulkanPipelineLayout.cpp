#include "VulkanPipelineLayout.h"

namespace Core
{
	void VulkanPipelineLayout::PushDescriptorSetLayout(VulkanDescriptorSetLayout descriptorSetLayout)
	{
		m_descriptorSetLayouts.push_back(descriptorSetLayout.GetHandle());
	}

	void VulkanPipelineLayout::Build(VkDevice device)
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32>(m_descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();

		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
	}

	VkPipelineLayout VulkanPipelineLayout::GetPipelineLayoutHandle()
	{
		return m_pipelineLayout;
	}
}