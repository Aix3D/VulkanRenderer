#pragma once
#include "VulkanBase.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanVertexInputState
	{
	private:
		ctd::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
		ctd::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
		VkPipelineVertexInputStateCreateInfo m_pipelineVertexInputStateCreateInfo;

	public:
		void PushBindingDescription(uint32 binding, uint32 stride, VkVertexInputRate inputState);
		void PushAttributeDescription(uint32 binding, uint32 location, VkFormat format, uint32 offset);
		void Build(VkDevice device);
		VkPipelineVertexInputStateCreateInfo GetHandle() const;
	};
}