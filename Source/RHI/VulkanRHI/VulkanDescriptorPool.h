#pragma once
#include "VulkanBase.h"
#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanDescriptorPool
	{
	private:
		ctd::vector<VkDescriptorPoolSize> m_poolSizes;
		VkDescriptorPool m_descriptorPool;

	public:
		void PushPoolSize(VkDescriptorType type, uint32 descriptorCount);
		void Build(VkDevice device);
		VkDescriptorPool GetHandle() const;
	};
}