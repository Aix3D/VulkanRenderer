#pragma once
#include <BaseInclude.h>
#include "VulkanBase.h"
#include "VulkanPipelineLayout.h"
#include "VulkanRenderPass.h"
#include "VulkanVertexInputState.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanPipeline
	{
	private:
		static VkPipelineCache pipelineCache;
		static Bool pipelineCacheCreated;
		ctd::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos ;
		VkPipeline m_pipeline;

	public:
		VulkanPipeline();
		void PushShader(VkDevice device, VkShaderStageFlagBits stageFlagBits, ANSICHAR * pShaderCode, size_t length);
		void Build(VkDevice device, VulkanVertexInputState inputState, VulkanPipelineLayout & pipelineLayout, VulkanRenderPass & renderPass);
		VkPipeline GetHandle();
		~VulkanPipeline();
	};
}