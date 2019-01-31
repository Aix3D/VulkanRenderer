#include "VulkanPipeline.h"
#include VECTOR_INCLUDE_PATH

using namespace ctd;

namespace Core
{
	VulkanPipeline::VulkanPipeline()
	{
		//VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
		//pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		//pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		//pipelineInputAssemblyStateCreateInfo.flags = 0;
		//pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = 0;
		//
		//VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
		//pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		//pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		//pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		//pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		//pipelineRasterizationStateCreateInfo.flags = 0;
		//pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		//pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
		//
		//VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
		//pipelineColorBlendAttachmentState.colorWriteMask = 0xf;
		//pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
		//
		//VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
		//pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		//pipelineColorBlendStateCreateInfo.attachmentCount = 1;
		//pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
		//
		//VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
		//pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		//pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		//pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		//pipelineDepthStencilStateCreateInfo.front = pipelineDepthStencilStateCreateInfo.back;
		//pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
		//
		//VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
		//pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		//pipelineViewportStateCreateInfo.viewportCount = 1;
		//pipelineViewportStateCreateInfo.scissorCount = 1;
		//pipelineViewportStateCreateInfo.flags = 0;
		//
		//VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
		//pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		//pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		//pipelineMultisampleStateCreateInfo.flags = 0;
		//
		//vector<VkDynamicState> dynamicStateEnables =
		//{
		//	VK_DYNAMIC_STATE_VIEWPORT,
		//	VK_DYNAMIC_STATE_SCISSOR
		//};
		//
		//VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
		//pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		//pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();
		//pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateEnables.size();
		//pipelineDynamicStateCreateInfo.flags = 0;
		//
		//// Solid rendering pipeline
		//// Load shaders
		//array<VkPipelineShaderStageCreateInfo, 2> shaderStages;
		//
		//shaderStages[0] = loadShader("mesh.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		//shaderStages[1] = loadShader("mesh.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
		//
		//VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		//pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		//pipelineCreateInfo.layout = m_pipelineLayout;
		//pipelineCreateInfo.renderPass = m_renderPass.Get();
		//pipelineCreateInfo.flags = 0;
		//pipelineCreateInfo.basePipelineIndex = -1;
		//pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		//
		//pipelineCreateInfo.pVertexInputState = &vertices.inputState;
		//pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
		//pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
		//pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
		//pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
		//pipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
		//pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
		//pipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
		//pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		//pipelineCreateInfo.pStages = shaderStages.data();
		//
		//VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_logicalDevice, m_pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_pipeline));
		//
		//// Wire frame rendering pipeline
		////if (deviceFeatures.fillModeNonSolid) {
		////	rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
		////	rasterizationState.lineWidth = 1.0f;
		////	VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines.wireframe));
		////}
	}

	VulkanPipeline::~VulkanPipeline()
	{

	}
}