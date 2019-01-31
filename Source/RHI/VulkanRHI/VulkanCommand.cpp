#include "VulkanCommand.h"

namespace Core
{
	Core::CommandType Command::GetCommandType()
	{
		return m_commandType;
	}

	Core::uint32 Command::GetCommandID()
	{
		return m_commandID;
	}

	BindVertexBuffers::BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
	{
		this->firstBinding = firstBinding;
		this->bindingCount = bindingCount;
		this->pBuffers = pBuffers;
		this->pOffsets = new VkDeviceSize[1]{ *pOffsets };
	}

	void BindVertexBuffers::Excute(VkCommandBuffer commandBuffer)
	{
		vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
	}

	BindIndexBuffer::BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
	{
		this->buffer = buffer;
		this->offset = offset;
		this->indexType = indexType;
	}

	void BindIndexBuffer::Excute(VkCommandBuffer commandBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer,buffer, offset, indexType);
	}

	DrawIndexed::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	{
		this->indexCount = indexCount;
		this->instanceCount = instanceCount;
		this->firstIndex = firstIndex;
		this->vertexOffset = vertexOffset;
		this->firstInstance = firstInstance;
	}

	void DrawIndexed::Excute(VkCommandBuffer commandBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}
}