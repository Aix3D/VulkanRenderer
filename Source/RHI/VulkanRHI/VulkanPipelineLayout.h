#pragma once
#include "VulkanBase.h"
#include "VulkanDescriptorSetLayout.h"
#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanPipelineLayout
	{
	private:
		ctd::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
		VkPipelineLayout m_pipelineLayout;

	public:
		void PushDescriptorSetLayout(VulkanDescriptorSetLayout descriptorSetLayout);
		void Build(VkDevice device);
		VkPipelineLayout GetPipelineLayoutHandle();
	};
}