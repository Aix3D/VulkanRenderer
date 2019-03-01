#pragma once
#include "VulkanBase.h"
#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanPipelineLayout
	{
	private:
		VkPipelineLayout m_pipelineLayout;

	public:
		void Build(VkDevice device, ctd::vector<VkDescriptorSetLayout> descriptorSetLayouts);
		VkPipelineLayout GetPipelineLayoutHandle();
	};
}