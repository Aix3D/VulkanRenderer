#include "VulkanPipelineLayout.h"

namespace Core
{
	void VulkanPipelineLayout::Build(VkDevice device, ctd::vector<VkDescriptorSetLayout> descriptorSetLayouts)
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayouts.size();
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();

		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
	}

	VkPipelineLayout VulkanPipelineLayout::GetPipelineLayoutHandle()
	{
		return m_pipelineLayout;
	}
}