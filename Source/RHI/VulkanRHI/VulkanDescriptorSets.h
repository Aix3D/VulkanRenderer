#pragma once
#include "VulkanBase.h"
#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanDescriptorSets
	{
	private:
		VkDescriptorPool m_descriptorPool;
		ctd::vector<VkDescriptorSetLayout> m_layouts;
		ctd::vector<VkDescriptorBufferInfo> m_bufferInfos;
		ctd::vector<VkDescriptorSet> m_descriptorSets;

	public:
		void PushDescriptorSetLayout(VkDescriptorSetLayout layout, VkDescriptorBufferInfo bufferInfo);
		void Build(VkDevice device);
		VkDescriptorSet * GetHandle();
		uint32 GetHandleCount();
	};
}