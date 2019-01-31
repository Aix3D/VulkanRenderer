#pragma once
#include "VulkanBase.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanPipelineLayout
	{
	private:
		ctd::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;

	public:
		void PushLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlag, uint32 binding);
	};
}