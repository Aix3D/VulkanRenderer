#pragma once
#include "VulkanBase.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanPipelineLayout
	{
	private:
		ctd::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkPipelineLayout m_pipelineLayout;

	public:
		void PushLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlag, uint32 binding);
		void Build(VkDevice device);
		VkDescriptorSetLayout GetDescriptorSetLayoutHandle();
		VkPipelineLayout GetPipelineLayoutHandle();
	};
}