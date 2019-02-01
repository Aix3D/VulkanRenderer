#pragma once
#include "VulkanBase.h"

namespace Core
{
	class VulkanRenderPass
	{
	private:
		VkRenderPass m_renderPass;

	public:
		VulkanRenderPass();
		ErrorCode Initialize(VkDevice logicalDevice, VkFormat colorFormat, VkFormat depthFormat);
		VkRenderPass Get() const;
		VkRenderPass GetHandle();
		~VulkanRenderPass();
	};
}