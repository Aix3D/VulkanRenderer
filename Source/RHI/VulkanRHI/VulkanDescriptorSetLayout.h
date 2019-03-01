#pragma once
#include "VulkanBase.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanDescriptorSetLayout
	{
	private:
		ctd::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;
		VkDescriptorSetLayout m_descriptorSetLayout;

	public:
		void PushLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlag, uint32 binding, uint32 descriptorCount);
		void Build(VkDevice device);
		VkDescriptorSetLayout GetHandle() const;
	};
}