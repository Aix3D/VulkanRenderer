#include "RenderSystem.h"

namespace Core
{
	RenderSystem::RenderSystem()
	{

	}

	void RenderSystem::Initialize(VulkanDevice * pDevice, GlobalRenderData * pGlobalRenderData, RenderableEntityData * pRenderableEntityData)
	{
		pDevice->CreateBuffer(
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			pRenderableEntityData->staticMesh->vertices.size() * sizeof(Vertex),
			&pRenderableEntityData->staticMesh->vertexBuffer,
			&pRenderableEntityData->staticMesh->vertexMemory,
			pRenderableEntityData->staticMesh->vertices.data());

		pDevice->CreateBuffer(
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			pRenderableEntityData->staticMesh->indices.size() * sizeof(uint32),
			&pRenderableEntityData->staticMesh->indexBuffer,
			&pRenderableEntityData->staticMesh->indexMemory,
			pRenderableEntityData->staticMesh->indices.data());

		if (!pRenderableEntityData->material->albedoTexture.expired())
			pRenderableEntityData->material->albedoTexture.lock()->UploadToGPU(pDevice);

		int32 featureID = pRenderableEntityData->material->GetFeatureID();

		if (pGlobalRenderData->descriptorTupleDict.find(featureID) == pGlobalRenderData->descriptorTupleDict.end())
		{
			pGlobalRenderData->descriptorTupleDict[featureID] = {};

			pGlobalRenderData->descriptorTupleDict[featureID].descriptorPool = {};

			pGlobalRenderData->descriptorTupleDict[featureID].descriptorPool.PushPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);

			if (!pRenderableEntityData->material->albedoTexture.expired())
				pGlobalRenderData->descriptorTupleDict[featureID].descriptorPool.PushPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);

			pGlobalRenderData->descriptorTupleDict[featureID].descriptorPool.Build(pDevice->GetLogicalDevice());

			//////////////////////////////////////////////////////////////////////////

			pGlobalRenderData->descriptorTupleDict[featureID].descriptorSetLayout = {};

			pGlobalRenderData->descriptorTupleDict[featureID].descriptorSetLayout.PushLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0, 1);

			if (!pRenderableEntityData->material->albedoTexture.expired())
				pGlobalRenderData->descriptorTupleDict[featureID].descriptorSetLayout.PushLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1);

			pGlobalRenderData->descriptorTupleDict[featureID].descriptorSetLayout.Build(pDevice->GetLogicalDevice());

			//////////////////////////////////////////////////////////////////////////
			pGlobalRenderData->descriptorTupleDict[featureID].pipelineLayout.PushDescriptorSetLayout(pGlobalRenderData->descriptorTupleDict[featureID].descriptorSetLayout);
			pGlobalRenderData->descriptorTupleDict[featureID].pipelineLayout.Build(pDevice->GetLogicalDevice());

			//	XXX:
			//		临时做法
			pGlobalRenderData->defaultVertexInputState.PushBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
			pGlobalRenderData->defaultVertexInputState.PushAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
			pGlobalRenderData->defaultVertexInputState.PushAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
			pGlobalRenderData->defaultVertexInputState.PushAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv0));
			pGlobalRenderData->defaultVertexInputState.PushAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
			pGlobalRenderData->defaultVertexInputState.Build(pDevice->GetLogicalDevice());

			pGlobalRenderData->defaultPipeline.PushShader(pDevice->GetLogicalDevice(), VK_SHADER_STAGE_VERTEX_BIT, pRenderableEntityData->material->vertexShader.lock()->pRawData.get(), pRenderableEntityData->material->vertexShader.lock()->size);
			pGlobalRenderData->defaultPipeline.PushShader(pDevice->GetLogicalDevice(), VK_SHADER_STAGE_FRAGMENT_BIT, pRenderableEntityData->material->fragmentShader.lock()->pRawData.get(), pRenderableEntityData->material->fragmentShader.lock()->size);

			pGlobalRenderData->defaultPipeline.Build(pDevice->GetLogicalDevice(), pGlobalRenderData->defaultVertexInputState, pGlobalRenderData->descriptorTupleDict[featureID].pipelineLayout, pDevice->GetDefaultRenderPass());
		}

		pRenderableEntityData->pipeline = pGlobalRenderData->defaultPipeline;

		pRenderableEntityData->descriptorSet.SetDescriptorSetLayout(pGlobalRenderData->descriptorTupleDict[featureID].descriptorSetLayout.GetHandle());
		pRenderableEntityData->descriptorSet.PushDescriptorInfo(pGlobalRenderData->uniformBuffer->descriptorBufferInfo);
		pRenderableEntityData->descriptorSet.PushDescriptorInfo(pRenderableEntityData->material->albedoTexture.lock()->descriptor);

		pRenderableEntityData->descriptorSet.Build(pDevice->GetLogicalDevice(), pGlobalRenderData->descriptorTupleDict[featureID].descriptorPool.GetHandle());
	
		//////////////////////////////////////////////////////////////////////////

		//	FIXME:
		//			为什么需要这样语法才对,待研究.
		VkDescriptorSet temp = pRenderableEntityData->descriptorSet.GetHandle();
		BindDescriptorSets * bindDescriptorSets = new BindDescriptorSets(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pGlobalRenderData->descriptorTupleDict[featureID].pipelineLayout.GetPipelineLayoutHandle(),
			0,
			1,
			temp,
			0,
			Null
			);

		BindPipeline * bindPipeline = new BindPipeline(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pGlobalRenderData->defaultPipeline.GetHandle()
		);

		VkDeviceSize offsets[1] = { 0 };

		BindVertexBuffers * bindVertexBuffers = new BindVertexBuffers(
			0,
			1,
			&pRenderableEntityData->staticMesh->vertexBuffer,
			offsets);

		BindIndexBuffer * bindIndexBuffer = new BindIndexBuffer(
			pRenderableEntityData->staticMesh->indexBuffer,
			0,
			VK_INDEX_TYPE_UINT32);

		DrawIndexed * drawIndexed = new DrawIndexed(
			static_cast<uint32>(pRenderableEntityData->staticMesh->indices.size()),
			1,
			0,
			0,
			0);

		pDevice->AddCommand(bindDescriptorSets);
		pDevice->AddCommand(bindPipeline);
		pDevice->AddCommand(bindVertexBuffers);
		pDevice->AddCommand(bindIndexBuffer);
		pDevice->AddCommand(drawIndexed);
	}

	void RenderSystem::Tick(float deltaTime, const RenderableEntityData * pData)
	{

	}

	RenderSystem::~RenderSystem()
	{

	}
}