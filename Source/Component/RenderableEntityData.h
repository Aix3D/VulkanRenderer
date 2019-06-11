#pragma once
#include "IComponent.h"
#include "../RHI/VulkanRHI/VulkanPipeline.h"
#include "../RHI/VulkanRHI/VulkanDescriptorPool.h"
#include "../RHI/VulkanRHI/VulkanDescriptorSetLayout.h"
#include "../RHI/VulkanRHI/VulkanDescriptorSet.h"
#include "../Asset/StaticMesh.h"
#include "../Asset/Material.h"

namespace Core
{
	struct RenderableEntityData : public IComponent
	{
		//std::weak_ptr<VulkanDescriptorPool> descriptorPool;
		//std::weak_ptr<VulkanDescriptorSetLayout> descriptorSetLayout;
		//std::weak_ptr<VulkanPipelineLayout> pipelineLayout;

		std::shared_ptr<StaticMesh> staticMesh;
		std::shared_ptr<Material> material;

		VulkanDescriptorSet descriptorSet;
		VulkanPipeline pipeline;
	};
}