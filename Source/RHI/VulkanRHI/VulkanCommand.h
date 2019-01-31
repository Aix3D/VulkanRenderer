#pragma once
#include <BaseInclude.h>
#include <vulkan\vulkan.h>

namespace Core
{
	enum CommandType
	{
		CommandType_BindVertexBuffer = 0,
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