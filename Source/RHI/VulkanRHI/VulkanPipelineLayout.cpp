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

}