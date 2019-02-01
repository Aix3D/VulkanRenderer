#pragma once
#include "VulkanBase.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanBuffer.h"
#include "VulkanCommand.h"
#include "VulkanCommandBufferPool.h"
#include "VulkanBufferManager.h"
#include "VulkanBufferP.h"
#include "VulkanVertexInputState.h"
#include "VulkanPipelineLayout.h"
#include "..\..\Component\RawShader.h"
#include "VulkanPipeline.h"

#include VECTOR_INCLUDE_PATH
#include STRING_INCLUDE_PATH

namespace Core
{
	//struct Model {
	//	struct {
	//		VkBuffer buffer;
	//		VkDeviceMemory memory;
	//	} vertices;
	//	struct {
	//		int count;
	//		VkBuffer buffer;
	//		VkDeviceMemory memory;
	//	} indices;
	//	// Destroys all Vulkan resources created for this model
	//	void destroy(VkDevice device)
	//	{
	//		vkDestroyBuffer(device, vertices.buffer, nullptr);
	//		vkFreeMemory(device, vertices.memory, nullptr);
	//		vkDestroyBuffer(device, indices.buffer, nullptr);
	//		vkFreeMemory(device, indices.memory, nullptr);
	//	};
	//};

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

		///////////////////////////////////////////////////////////////////////////
		VkDevice m_logicalDevice;

		uint32 m_graphicsQueueFamilyIndex;
		uint32 m_computeQueueFamilyIndex;
		uint32 m_transferQueueFamilyIndex;

		VkQueueFlags m_requestedQueueTypes;
		ctd::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
		VkQueue m_queue;

		VkCommandPool m_commandPool;

		//VulkanCommandBufferPool m_vulaknCommandPool;
		//ctd::vector<std::unique_ptr<VulkanCommandBuffer>> m_vulkanCommandBuffers;

		VkFormat m_depthFormat;

		VulkanSwapChain m_swapChain;

		VkSemaphore m_presentCompleteSemaphore;
		VkSemaphore m_renderCompleteSemaphore;

		VkPipelineStageFlags m_submitPipelineStages;
		VkSubmitInfo m_submitInfo;

		ctd::vector<VkCommandBuffer> m_drawCommandBuffers;

		VkImage m_depthStencilImage;
		VkDeviceMemory m_depthStencilMemory;
		VkImageView m_depthStencilView;

		VulkanRenderPass m_renderPass;
		VkPipelineCache m_pipelineCache;

		ctd::vector<VkFramebuffer> m_frameBuffers;

		//VkDescriptorSetLayout m_descriptorSetLayout;
		//VkPipelineLayout m_pipelineLayout;

		//std::vector<VkShaderModule> m_shaderModules;

		//VkPipeline m_pipeline;

		VkDescriptorPool m_descriptorPool;
		VkDescriptorSet m_descriptorSet;

		uint32_t m_currentBuffer = 0;

		const ANSICHAR * sceneFullPath = "G:\\Scenes";

		ctd::vector<Command *> m_commandList;

		std::unique_ptr<VulkanBufferManager> m_pBufferManager;
		std::weak_ptr<VulkanBufferP> m_pUniformBuffer;

		VulkanVertexInputState m_inputState;
		VulkanPipelineLayout m_pipelineLayout;
		VulkanPipeline m_pipeline;

		///////////////////////////////////////////////////////////////////////////

		VkBool32 m_messageCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t srcObject,
			size_t location,
			int32_t msgCode,
			const char* pLayerPrefix,
			const char* pMsg,
			void* pUserData);

		PFN_vkCreateDebugReportCallbackEXT m_PFN_createDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDestroyDebugReportCallbackEXT m_PFN_destroyDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDebugReportMessageEXT m_PFN_dbgBreakCallback = VK_NULL_HANDLE;

		ErrorCode createVKInstance();
		uint32 getQueueFamilyIndex(VkQueueFlagBits queueFlagBits);
		Bool getSupportedDepthFormat(VkFormat *depthFormat);
		uint32 getMemoryType(uint32 typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr);

		//void setupVertexDescriptions();
		//void setupDescriptorSetLayout();
		//void preparePipelines();
		void setupDescriptorPool();
		void setupDescriptorSet();
		//VkPipelineShaderStageCreateInfo loadShader(ANSICHAR * fileName, VkShaderStageFlagBits stage);

		void initializeLogicalDevice(void* platformHandle, void* platformWindow);

		std::weak_ptr<RawShader> m_pVertexShaderData;
		std::weak_ptr<RawShader> m_pFragmentShaderData;

	public:
		VulkanDevice();
		ErrorCode Initialize(std::shared_ptr<RawShader> pVertexShader, std::shared_ptr<RawShader> pFragmentShader, void* platformHandle, void* platformWindow);
		void Draw();
		void CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer *buffer, VkDeviceMemory *memory, void *data /*= nullptr*/);
		VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level, bool begin);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer, bool free);

		void AddCommand(Command * pMommand);
		void RemoveCommand(uint32 commandID);
		void BuildCommandBuffers();
		~VulkanDevice();
	};
}