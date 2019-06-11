#pragma once
#include <BaseInclude.h>
#include <vulkan\vulkan.h>

namespace Core
{
	enum CommandType
	{
		CommandType_BindDescriptorSets = 0,
		CommandType_BindPipeline,
		CommandType_BindVertexBuffer,
		CommandType_BindIndexBuffer,
		CommandType_DrawIndexed,
		CommandType_Count,
	};

	class Command
	{
	private:
		CommandType m_commandType;
		uint32 m_commandID;

	public:
		CommandType GetCommandType();
		uint32 GetCommandID();
		virtual void Excute(VkCommandBuffer commandBuffer) = 0;
	};

	class BindDescriptorSets : public Command
	{
	private:
		VkPipelineBindPoint pipelineBindPoint;
		VkPipelineLayout pipelineLayout;
		uint32_t firstSet;
		uint32_t descriptorSetCount;
		VkDescriptorSet descriptorSets;
		uint32_t dynamicOffsetCount;
		const uint32_t* pDynamicOffsets;

	public:
		BindDescriptorSets(
			VkPipelineBindPoint pipelineBindPoint,
			VkPipelineLayout pipelineLayout,
			uint32_t firstSet,
			uint32_t descriptorSetCount,
			VkDescriptorSet descriptorSet,
			uint32_t dynamicOffsetCount,
			const uint32_t* pDynamicOffsets
			);
		virtual void Excute(VkCommandBuffer commandBuffer) override;
		virtual ~BindDescriptorSets() {};
	};

	class BindPipeline : public Command
	{
	private:
		VkPipelineBindPoint pipelineBindPoint;
		VkPipeline pipeline;
	public:
		BindPipeline(
			VkPipelineBindPoint pipelineBindPoint,
			VkPipeline pipeline
		);
		virtual void Excute(VkCommandBuffer commandBuffer) override;
		virtual ~BindPipeline() {};
	};

	class BindVertexBuffers : public Command
	{
	private:
		uint32_t firstBinding;
		uint32_t bindingCount;
		const VkBuffer* pBuffers;
		const VkDeviceSize* pOffsets;

	public:
		BindVertexBuffers(
			uint32_t firstBinding,
			uint32_t bindingCount,
			const VkBuffer* pBuffers,
			const VkDeviceSize* pOffsets
		);
		virtual void Excute(VkCommandBuffer commandBuffer) override;
		virtual ~BindVertexBuffers() {
			delete[] pOffsets
				;
		};
	};

	class BindIndexBuffer : public Command
	{
	private:
		VkBuffer buffer;
		VkDeviceSize offset;
		VkIndexType indexType;

	public:
		BindIndexBuffer(
			VkBuffer buffer,
			VkDeviceSize offset,
			VkIndexType indexType);
		virtual void Excute(VkCommandBuffer commandBuffer) override;
		virtual ~BindIndexBuffer() {};
	};

	class DrawIndexed : public Command
	{
	private:
		uint32_t  indexCount;
		uint32_t  instanceCount;
		uint32_t  firstIndex;
		int32_t   vertexOffset;
		uint32_t  firstInstance;

	public:
		DrawIndexed(
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t firstIndex,
			int32_t vertexOffset,
			uint32_t firstInstance);

		virtual void Excute(VkCommandBuffer commandBuffer) override;
		virtual ~DrawIndexed() {};
	};
}