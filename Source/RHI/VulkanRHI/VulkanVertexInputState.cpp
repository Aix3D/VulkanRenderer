#include "VulkanVertexInputState.h"

namespace Core
{
	void VulkanVertexInputState::PushBindingDescription(uint32 binding, uint32 stride, VkVertexInputRate inputState)
	{
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = binding;
		bindingDescription.stride = stride;
		bindingDescription.inputRate = inputState;

		m_bindingDescriptions.push_back(bindingDescription);
	}

	void VulkanVertexInputState::PushAttributeDescription(uint32 binding, uint32 location, VkFormat format, uint32 offset)
	{
		VkVertexInputAttributeDescription attributeDescription = {};

		attributeDescription.binding = binding;
		attributeDescription.location = location;
		attributeDescription.format = format;
		attributeDescription.offset = offset;

		m_attributeDescriptions.push_back(attributeDescription);
	}

	void VulkanVertexInputState::Build(VkDevice device)
	{
		m_pipelineVertexInputStateCreateInfo = {};
		m_pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		m_pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32>(m_bindingDescriptions.size());
		m_pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = m_bindingDescriptions.data();
		m_pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(m_attributeDescriptions.size());
		m_pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();
	}

	VkPipelineVertexInputStateCreateInfo VulkanVertexInputState::GetHandle() const
	{
		return m_pipelineVertexInputStateCreateInfo;
	}
}