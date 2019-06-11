#pragma once
#include "VulkanBase.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanBuffer.h"
#include "VulkanCommand.h"
#include "VulkanCommandBufferPool.h"
#include "VulkanBuffer.h"
#include "VulkanVertexInputState.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "..\..\Object\Camera.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "..\..\Asset\RawShader.h"

#include VECTOR_INCLUDE_PATH
#include STRING_INCLUDE_PATH

namespace Core
{
	VkBool32 messageCallback(
		VkDebugReportFlagsEXT                       flags,
		VkDebugReportObjectTypeEXT                  objectType,
		uint64_t                                    object,
		size_t                                      location,
		int32_t                                     messageCode,
		const char*                                 pLayerPrefix,
		const char*                                 pMessage,
		void*                                       pUserData);

	class VulkanDevice
	{
	private:
		VkInstance m_instance;
		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_deviceProperties;
		VkPhysicalDeviceFeatures m_deviceFeatures;
		VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;

		ctd::vector<ctd::string> m_supportedExtensions;

		uint32 m_width;
		uint32 m_height;

		ANSICHAR * m_pAppName ;
		ANSICHAR * m_pEngineName;

		Bool m_validation;
		VkDevice m_logicalDevice;

		uint32 m_graphicsQueueFamilyIndex;
		uint32 m_computeQueueFamilyIndex;
		uint32 m_transferQueueFamilyIndex;

		VkQueueFlags m_requestedQueueTypes;
		ctd::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
		VkQueue m_queue;

		VkCommandPool m_commandPool;

		VkFormat m_depthFormat;

		VulkanSwapChain m_swapChain;

		VkSemaphore m_presentCompleteSemaphore;
		VkSemaphore m_renderCompleteSemaphore;

		VkPipelineStageFlags m_submitPipelineStages;
		VkSubmitInfo m_submitInfo;

		ctd::vector<VkCommandBuffer> m_drawCommandBuffers;

		std::unique_ptr<VulkanImage> m_depthStencilImage;
		std::unique_ptr<VulkanImageView> m_depthStencilImageView;

		VulkanRenderPass m_renderPass;

		ctd::vector<VkFramebuffer> m_frameBuffers;

		uint32_t m_currentBuffer = 0;

		const ANSICHAR * sceneFullPath = "G:\\Scenes";

		ctd::vector<Command *> m_commandList;

		std::weak_ptr<VulkanBuffer> m_pUniformBuffer;

		VulkanVertexInputState m_inputState;
		VulkanDescriptorSetLayout m_descriptorSetLayout;
		VulkanDescriptorPool m_descriptorPool;
		VulkanDescriptorSet m_descriptorSet;

		VulkanPipelineLayout m_pipelineLayout;
		VulkanPipeline m_pipeline;

		PFN_vkCreateDebugReportCallbackEXT m_PFN_createDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDestroyDebugReportCallbackEXT m_PFN_destroyDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDebugReportMessageEXT m_PFN_dbgBreakCallback = VK_NULL_HANDLE;

		VkDebugReportCallbackEXT m_msgCallback;

		ErrorCode createVKInstance();
		uint32 getQueueFamilyIndex(VkQueueFlagBits queueFlagBits);
		Bool getSupportedDepthFormat(VkFormat *depthFormat);
		uint32 getMemoryType(uint32 typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr);

		void initializeLogicalDevice(void* platformHandle, void* platformWindow);

	public:
		VulkanDevice();
		VkDevice GetLogicalDevice() const;
		VulkanRenderPass GetDefaultRenderPass() const;
		ErrorCode Initialize(void* platformHandle, void* platformWindow);
		void PrepareSTH();
		void Draw();
		void CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer *buffer, VkDeviceMemory *memory, void *data /*= nullptr*/);
		std::unique_ptr<VulkanBuffer> CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size);
		VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level, bool begin);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer, bool free);

		void AddCommand(Command * pMommand);
		void RemoveCommand(uint32 commandID);
		void BuildCommandBuffers();

		//void UpdateCamera(Camera camera);
		VkFormatProperties GetPhysicalDeviceFormatProperties(VkFormat format) const;
		void SetImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageAspectFlags aspectMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		VkQueue GetQueue() const;

		~VulkanDevice();
	};
}