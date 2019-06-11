#pragma once
#include "IComponent.h"
#include "../RHI/VulkanRHI/VulkanBuffer.h"
#include "../RHI/VulkanRHI/VulkanDescriptorPool.h"
#include "../RHI/VulkanRHI/VulkanDescriptorSetLayout.h"
#include "../RHI/VulkanRHI/VulkanPipelineLayout.h"
#include "../RHI/VulkanRHI/VulkanVertexInputState.h"
#include "../RHI/VulkanRHI/VulkanPipeline.h"
#include "../DesignPattern/Singleton.h"
#include "../Math/Matrix4x4.h"

#include MAP_INCLUDE_PATH

namespace Core
{
	struct DescriptorTuple
	{
		VulkanDescriptorPool descriptorPool;
		VulkanDescriptorSetLayout descriptorSetLayout;
		VulkanPipelineLayout pipelineLayout;
	};

	struct RawUniformBuffer
	{
		Matrix4x4 projection;
		Matrix4x4 model;
	};

	struct GlobalRenderData : public Singleton<GlobalRenderData>, public IComponent
	{
		ctd::map<int32, DescriptorTuple> descriptorTupleDict;

		VulkanVertexInputState defaultVertexInputState;
		VulkanPipeline defaultPipeline;

		std::unique_ptr<VulkanBuffer> uniformBuffer;
		RawUniformBuffer rawUniformBuffer; 
	};
}