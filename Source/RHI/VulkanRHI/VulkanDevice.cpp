#include "VulkanDevice.h"
#include "..\..\Helper\AssetManager.h"
#include "..\..\Math\Vector3.h"
#include "..\..\Math\Vector2.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "VulkanBuffer.h"
#include "..\..\Math\Matrix4x4.h"
#include "..\..\Math\Vector4.h"
#include "..\..\Math\MathGlobal.h"

#include <fstream>
#include <io.h>
#include "VulkanGlobal.h"

#include <sstream>
#include <iostream>

#include ARRAY_INCLUDE_PATH

#define VERTEX_BUFFER_BIND_ID 0

using namespace ctd;

namespace Core
{
	VkBool32 messageCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
	{
		// Select prefix depending on flags passed to the callback
		// Note that multiple flags may be set for a single validation message
		std::string prefix("");

		// Error that may result in undefined behaviour
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			prefix += "ERROR:";
		};
		// Warnings may hint at unexpected / non-spec API usage
		if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			prefix += "WARNING:";
		};
		// May indicate sub-optimal usage of the API
		if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			prefix += "PERFORMANCE:";
		};
		// Informal messages that may become handy during debugging
		if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		{
			prefix += "INFO:";
		}
		// Diagnostic info from the Vulkan loader and layers
		// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
		if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		{
			prefix += "DEBUG:";
		}

		// Display message to default output (console/logcat)
		std::stringstream debugMessage;
		debugMessage << prefix << " [" << pLayerPrefix << "] Code " << messageCode << " : " << pMessage;

		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			std::cerr << debugMessage.str() << "\n";
		}
		else {
			std::cout << debugMessage.str() << "\n";
		}

		fflush(stdout);

		// The return value of this callback controls wether the Vulkan call that caused
		// the validation message will be aborted or not
		// We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message 
		// (and return a VkResult) to abort
		// If you instead want to have calls abort, pass in VK_TRUE and the function will 
		// return VK_ERROR_VALIDATION_FAILED_EXT 
		return VK_FALSE;
	}

	struct {
		Matrix4x4 projection;
		Matrix4x4 model;
		Vector4 lightPos = Vector4(25.0f, 5.0f, 5.0f, 1.0f);
	} uboVS;

	ErrorCode VulkanDevice::createVKInstance()
	{
		VkApplicationInfo applicationInfo = {};

		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = m_pAppName;
		applicationInfo.pEngineName = m_pEngineName;
		applicationInfo.apiVersion = VK_API_VERSION_1_0;

		vector<const ANSICHAR*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

#if defined (PLATFORM_WINDOWS)
		instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined (PLATFORM_ANDROID)
		instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

		if (m_validation)
			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

		VkInstanceCreateInfo instanceCreateInfo = {};

		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = NULL;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;

		instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (m_validation)
		{
			static const ANSICHAR *validationLayerNames[] = {
				"VK_LAYER_LUNARG_standard_validation"
			};

			instanceCreateInfo.enabledLayerCount = 1;
			instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
		}

		VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance));

		return ErrorCode_OK;
	}

	Core::uint32 VulkanDevice::getQueueFamilyIndex(VkQueueFlagBits queueFlagBits)
	{
		//	compute only
		if (queueFlagBits & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32 i = 0; i < m_queueFamilyProperties.size(); ++i)
			{
				if ((m_queueFamilyProperties[i].queueFlags & queueFlagBits) &&
					((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					return i;
				}
			}
		}

		// transfer only
		if (queueFlagBits & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32 i = 0; i < m_queueFamilyProperties.size(); ++i)
			{
				if ((m_queueFamilyProperties[i].queueFlags & queueFlagBits) &&
					((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
					((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					return i;
				}
			}
		}

		for (uint32 i = 0; i < m_queueFamilyProperties.size(); ++i)
		{
			if (m_queueFamilyProperties[i].queueFlags & queueFlagBits)
			{
				return i;
			}
		}

		return InvalidIndexUint32;
	}

	Core::Bool VulkanDevice::getSupportedDepthFormat(VkFormat *depthFormat)
	{
		std::vector<VkFormat> depthFormats =
		{
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for (auto& format : depthFormats)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProps);

			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				*depthFormat = format;

				return True;
			}
		}

		return False;
	}

	Core::uint32 VulkanDevice::getMemoryType(uint32 typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound /*= nullptr*/)
	{
		for (uint32 i = 0; i < m_deviceMemoryProperties.memoryTypeCount; ++i)
		{
			if ((typeBits & 1) == 1)
			{
				if ((m_deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					if (memTypeFound)
					{
						*memTypeFound = true;
					}

					return i;
				}
			}

			typeBits >>= 1;
		}

		if (memTypeFound)
		{
			*memTypeFound = false;
			return 0;
		}
		else
		{
			throw std::runtime_error("Could not find a matching memory type");
		}
	}

	void VulkanDevice::initializeLogicalDevice(void* platformHandle, void* platformWindow)
	{
		//	Queue family orioerties must be intialized ahead,
		//	getQueueFamilyIndex() will uses it later.
		uint32 queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

		m_queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_queueFamilyProperties.data());

		vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
		const float defaultQueuePriority = 0;

		if (m_requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
		{
			m_graphicsQueueFamilyIndex = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);

			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
		else
			m_graphicsQueueFamilyIndex = VK_NULL_HANDLE;

		if (m_requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
		{
			m_computeQueueFamilyIndex = getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);

			if (m_computeQueueFamilyIndex != m_graphicsQueueFamilyIndex)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = m_computeQueueFamilyIndex;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}
		}
		else
			m_computeQueueFamilyIndex = m_graphicsQueueFamilyIndex;

		if (m_requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
		{
			m_transferQueueFamilyIndex = getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);

			if ((m_transferQueueFamilyIndex != m_graphicsQueueFamilyIndex) &&
				(m_transferQueueFamilyIndex != m_computeQueueFamilyIndex))
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = m_transferQueueFamilyIndex;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}
		}
		else
		{
			m_transferQueueFamilyIndex = m_graphicsQueueFamilyIndex;
		}

		vector<ANSICHAR*> deviceExtensions;

		//	if use swapchain
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &m_deviceFeatures;

		if ((ctd::find(m_supportedExtensions.begin(), m_supportedExtensions.end(), VK_EXT_DEBUG_MARKER_EXTENSION_NAME) != m_supportedExtensions.end()))
			deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

		if (deviceExtensions.size() > 0)
		{
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		}

		VK_CHECK_RESULT(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_logicalDevice));

		m_pBufferManager = std::make_unique<VulkanBufferManager>(m_logicalDevice, m_deviceMemoryProperties);

		vkGetDeviceQueue(m_logicalDevice, m_graphicsQueueFamilyIndex, 0, &m_queue);

		VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
		cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolCreateInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
		cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(m_logicalDevice, &cmdPoolCreateInfo, nullptr, &m_commandPool));
	
		getSupportedDepthFormat(&m_depthFormat);

		m_swapChain.Connect(m_instance, m_physicalDevice, m_logicalDevice);
		m_swapChain.InitializeSurface(platformHandle, platformWindow, m_queueFamilyProperties);
		m_swapChain.Prepare(&m_width, &m_height, True);
	}

	VulkanDevice::VulkanDevice()
	{
		m_pAppName = "Vulkan Renderer";
		m_pEngineName = "Reserved";

		m_requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

		m_submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		m_validation = True;
	}

	Core::ErrorCode VulkanDevice::Initialize(std::shared_ptr<RawShader> pVertexShader, std::shared_ptr<RawShader> pFragmentShader, void* platformHandle, void* platformWindow)
	{
		m_pVertexShaderData = pVertexShader;
		m_pFragmentShaderData = pFragmentShader;

		createVKInstance();

		if (m_validation)
		{
			//	TODO:	implement later.
			m_PFN_createDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT"));
			m_PFN_destroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"));
			m_PFN_dbgBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(m_instance, "vkDebugReportMessageEXT"));

			VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
			dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback;
			dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;

			VkResult err = m_PFN_createDebugReportCallback(
				m_instance,
				&dbgCreateInfo,
				nullptr,
				&m_msgCallback);
		}

		uint32 gpuCount;
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_instance, &gpuCount, nullptr));

		vector<VkPhysicalDevice> physicalDevices(gpuCount);
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_instance, &gpuCount, physicalDevices.data()));
	
		m_physicalDevice = physicalDevices[0];

		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_deviceProperties);
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_deviceMemoryProperties);

		VulkanGlobal::Instance()->deviceMemoryProperties = m_deviceMemoryProperties;

		uint32 extensionCount;
		VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, nullptr));

		if (extensionCount > 0)
		{
			vector<VkExtensionProperties> extensions(extensionCount);

			VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, &extensions.front()));
		
			for (auto item : extensions)
			{
				m_supportedExtensions.push_back(item.extensionName);
			}
		}


		initializeLogicalDevice(platformHandle, platformWindow);


		return ErrorCode_OK;
	}

	void VulkanDevice::PrepareSTH(VkDescriptorImageInfo imageInfo)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VK_CHECK_RESULT(vkCreateSemaphore(m_logicalDevice, &semaphoreCreateInfo, nullptr, &m_presentCompleteSemaphore));
		VK_CHECK_RESULT(vkCreateSemaphore(m_logicalDevice, &semaphoreCreateInfo, nullptr, &m_renderCompleteSemaphore));

		m_submitInfo = {};
		m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		m_submitInfo.pWaitDstStageMask = &m_submitPipelineStages;
		m_submitInfo.waitSemaphoreCount = 1;
		m_submitInfo.pWaitSemaphores = &m_presentCompleteSemaphore;
		m_submitInfo.signalSemaphoreCount = 1;
		m_submitInfo.pSignalSemaphores = &m_renderCompleteSemaphore;

		m_drawCommandBuffers.resize(m_swapChain.GetImageCount());

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = m_commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = static_cast<uint32>(m_drawCommandBuffers.size());
		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_logicalDevice, &commandBufferAllocateInfo, m_drawCommandBuffers.data()));

		m_depthStencilImage = std::make_unique<VulkanImage>(
			m_logicalDevice,
			m_depthFormat,
			m_width,
			m_height,
			1,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
			);

		m_depthStencilImageView = std::make_unique<VulkanImageView>(
			m_logicalDevice,
			m_depthFormat,
			VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
			m_depthStencilImage->GetHandle()
			);

		m_renderPass.Initialize(m_logicalDevice, m_swapChain.GetColorFormat(), m_depthFormat);

		VkImageView attachments[2];

		attachments[1] = m_depthStencilImageView->GetHandle();

		VkFramebufferCreateInfo frameBufferCreateInfo = {};
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.pNext = NULL;
		frameBufferCreateInfo.renderPass = m_renderPass.Get();
		frameBufferCreateInfo.attachmentCount = 2;
		frameBufferCreateInfo.pAttachments = attachments;
		frameBufferCreateInfo.width = m_width;
		frameBufferCreateInfo.height = m_height;
		frameBufferCreateInfo.layers = 1;

		// Create frame buffers for every swap chain image
		m_frameBuffers.resize(m_swapChain.GetImageCount());
		for (uint32_t i = 0; i < m_frameBuffers.size(); ++i)
		{
			attachments[0] = m_swapChain.GetView(i);
			VK_CHECK_RESULT(vkCreateFramebuffer(m_logicalDevice, &frameBufferCreateInfo, nullptr, &m_frameBuffers[i]));
		}

		//setupVertexDescriptions();

		m_inputState.PushBindingDescription(0, sizeof(VertexP), VK_VERTEX_INPUT_RATE_VERTEX);
		m_inputState.PushAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexP, position));
		m_inputState.PushAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexP, normal));
		m_inputState.PushAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexP, uv0));
		m_inputState.PushAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexP, color));
		m_inputState.Build(m_logicalDevice);

		m_pUniformBuffer = m_pBufferManager->CreateBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			sizeof(uboVS));

		m_pUniformBuffer.lock()->SetDescriptorBufferInfo();

		uboVS.projection = Perspective(90.0f * Deg2Rad, (float)m_width / (float)m_height, 0.1f, 256.0f);
		uboVS.model = Translate(Matrix4x4Identify, Vector3(0.0f, 0.0f, -8.0f));

		m_pUniformBuffer.lock()->Map(m_logicalDevice, 0, sizeof(uboVS));

		memcpy(m_pUniformBuffer.lock()->mapped, &uboVS, sizeof(uboVS));

		m_pUniformBuffer.lock()->Unmap(m_logicalDevice, sizeof(uboVS));

		m_descriptorPool.PushPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
		m_descriptorPool.PushPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		m_descriptorPool.Build(m_logicalDevice);

		m_descriptorSetLayout.PushLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0, 1);
		m_descriptorSetLayout.PushLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1);
		m_descriptorSetLayout.Build(m_logicalDevice);

		vector<VkDescriptorSetLayout> descriptorSetLayoutHandles;
		descriptorSetLayoutHandles.push_back(m_descriptorSetLayout.GetHandle());

		m_pipelineLayout.Build(m_logicalDevice, descriptorSetLayoutHandles);

		m_pipeline.PushShader(m_logicalDevice, VK_SHADER_STAGE_VERTEX_BIT, *m_pVertexShaderData.lock()->pRawData, m_pVertexShaderData.lock()->size);
		m_pipeline.PushShader(m_logicalDevice, VK_SHADER_STAGE_FRAGMENT_BIT, *m_pFragmentShaderData.lock()->pRawData, m_pFragmentShaderData.lock()->size);

		m_pipeline.Build(m_logicalDevice, m_inputState, m_pipelineLayout, m_renderPass);

		m_descriptorSet.SetDescriptorSetLayout(m_descriptorSetLayout.GetHandle());
		m_descriptorSet.PushDescriptorInfo(m_pUniformBuffer.lock()->descriptorBufferInfo);
		m_descriptorSet.PushDescriptorInfo(imageInfo);
		
		m_descriptorSet.Build(m_logicalDevice, m_descriptorPool.GetHandle());
	}

	void VulkanDevice::Draw()
	{
		m_swapChain.AcquireNextImage(m_presentCompleteSemaphore, &m_currentBuffer);

		m_submitInfo.commandBufferCount = 1;
		m_submitInfo.pCommandBuffers = &m_drawCommandBuffers[m_currentBuffer];

		// Submit to queue
		VK_CHECK_RESULT(vkQueueSubmit(m_queue, 1, &m_submitInfo, VK_NULL_HANDLE));

		m_swapChain.QueuePresent(m_queue, m_currentBuffer, m_renderCompleteSemaphore);

		vkQueueWaitIdle(m_queue);
	}

	void VulkanDevice::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer *buffer, VkDeviceMemory *memory, void *data /*= nullptr*/)
	{
		// Create the buffer handle
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.usage = usageFlags;
		bufferCreateInfo.size = size;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK_RESULT(vkCreateBuffer(m_logicalDevice, &bufferCreateInfo, nullptr, buffer));

		// Create the memory backing up the buffer handle
		VkMemoryRequirements memReqs;
		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vkGetBufferMemoryRequirements(m_logicalDevice, *buffer, &memReqs);
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);
		VK_CHECK_RESULT(vkAllocateMemory(m_logicalDevice, &memAllocInfo, nullptr, memory));

		// If a pointer to the buffer data has been passed, map the buffer and copy over the data
		if (data != nullptr)
		{
			void *mapped;
			VK_CHECK_RESULT(vkMapMemory(m_logicalDevice, *memory, 0, size, 0, &mapped));
			memcpy(mapped, data, size);
			// If host coherency hasn't been requested, do a manual flush to make writes visible
			if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
			{
				VkMappedMemoryRange mappedRange{};
				mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;

				mappedRange.memory = *memory;
				mappedRange.offset = 0;
				mappedRange.size = size;
				vkFlushMappedMemoryRanges(m_logicalDevice, 1, &mappedRange);
			}

			vkUnmapMemory(m_logicalDevice, *memory);
		}

		// Attach the memory to the buffer object
		VK_CHECK_RESULT(vkBindBufferMemory(m_logicalDevice, *buffer, *memory, 0));
	}

	VkCommandBuffer VulkanDevice::CreateCommandBuffer(VkCommandBufferLevel level, bool begin)
	{
		VkCommandBuffer commandBuffer;

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = m_commandPool;
		commandBufferAllocateInfo.level = level;
		commandBufferAllocateInfo.commandBufferCount = 1;

		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_logicalDevice, &commandBufferAllocateInfo, &commandBuffer));

		// If requested, also start the new command buffer
		if (begin)
		{
			VkCommandBufferBeginInfo commanddBufferBeginInfo{};
			commanddBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &commanddBufferBeginInfo));
		}

		return commandBuffer;
	}

	void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer, bool free)
	{
		if (commandBuffer == VK_NULL_HANDLE)
		{
			return;
		}

		VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VK_CHECK_RESULT(vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE));
		VK_CHECK_RESULT(vkQueueWaitIdle(m_queue));

		if (free)
			vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
	}

	void VulkanDevice::AddCommand(Command * pMommand)
	{
		m_commandList.push_back(pMommand);
	}

	void VulkanDevice::RemoveCommand(uint32 commandID)
	{
		
	}

	void VulkanDevice::BuildCommandBuffers()
	{
		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.025f, 0.025f, 0.025f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_renderPass.Get();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = m_width;
		renderPassBeginInfo.renderArea.extent.height = m_height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		for (int32_t i = 0; i < m_drawCommandBuffers.size(); ++i)
		{
			// Set target frame buffer
			renderPassBeginInfo.framebuffer = m_frameBuffers[i];

			VK_CHECK_RESULT(vkBeginCommandBuffer(m_drawCommandBuffers[i], &cmdBufferBeginInfo));

			vkCmdBeginRenderPass(m_drawCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport{};
			viewport.width = static_cast<float>(m_width);
			viewport.height = static_cast<float>(m_height);
			viewport.minDepth = 0;
			viewport.maxDepth = 1.0;

			vkCmdSetViewport(m_drawCommandBuffers[i], 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.extent.width = m_width;
			scissor.extent.height = m_height;
			scissor.offset.x = 0;
			scissor.offset.y = 0;

			vkCmdSetScissor(m_drawCommandBuffers[i], 0, 1, &scissor);

			VkDescriptorSet temp = m_descriptorSet.GetHandle();
			vkCmdBindDescriptorSets(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.GetPipelineLayoutHandle(), 0, 1, &temp, 0, NULL);
			vkCmdBindPipeline(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetHandle());

			//VkDeviceSize offsets[1] = { 0 };

			for (vector<Command *>::iterator iter = m_commandList.begin();
				iter != m_commandList.end();
				++iter)
			{
				(*iter)->Excute(m_drawCommandBuffers[i]);
			}

			// Bind mesh vertex buffer
			//vkCmdBindVertexBuffers(m_drawCommandBuffers[i], VERTEX_BUFFER_BIND_ID, 1, &model.vertices.buffer, offsets);
			//// Bind mesh index buffer
			//vkCmdBindIndexBuffer(m_drawCommandBuffers[i], model.indices.buffer, 0, VK_INDEX_TYPE_UINT32);
			//// Render mesh vertex buffer using it's indices
			//vkCmdDrawIndexed(m_drawCommandBuffers[i], model.indices.count, 1, 0, 0, 0);

			vkCmdEndRenderPass(m_drawCommandBuffers[i]);

			VK_CHECK_RESULT(vkEndCommandBuffer(m_drawCommandBuffers[i]));
		}
	}

	void VulkanDevice::UpdateCamera(Camera camera)
	{		
		uboVS.model = camera.m_viewMatrix;
		uboVS.projection = camera.m_perspectiveMatrix;

		if (!m_pUniformBuffer.lock()->mapped)
			m_pUniformBuffer.lock()->Map(m_logicalDevice, 0, sizeof(uboVS));

		memcpy(m_pUniformBuffer.lock()->mapped, &uboVS, sizeof(uboVS));

		//m_pUniformBuffer.lock()->Unmap(m_logicalDevice, sizeof(uboVS));
	}

	VkFormatProperties VulkanDevice::GetPhysicalDeviceFormatProperties(VkFormat format) const
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProperties);

		return formatProperties;
	}

	VkDevice VulkanDevice::GetLogicalDevice()
	{
		return m_logicalDevice;
	}

	void VulkanDevice::SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask /*= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT*/, VkPipelineStageFlags dstStageMask /*= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT*/)
	{
		// Create an image barrier object
		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		
		imageMemoryBarrier.oldLayout = oldImageLayout;
		imageMemoryBarrier.newLayout = newImageLayout;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;

		// Source layouts (old)
		// Source access mask controls actions that have to be finished on the old layout
		// before it will be transitioned to the new layout
		switch (oldImageLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			imageMemoryBarrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			// Image is preinitialized
			// Only valid as initial layout for linear images, preserves memory contents
			// Make sure host writes have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image is a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image is a depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source 
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image is read by a shader
			// Make sure any shader reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		}

		// Target layouts (new)
		// Destination access mask controls the dependency for the new image layout
		switch (newImageLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from and writes to the image have been finished
			imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image will be used as a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image layout will be used as a depth/stencil attachment
			// Make sure any writes to depth/stencil buffer have been finished
			imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (imageMemoryBarrier.srcAccessMask == 0)
			{
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		}

		// Put barrier inside setup command buffer
		vkCmdPipelineBarrier(
			cmdbuffer,
			srcStageMask,
			dstStageMask,
			0,
			0, nullptr,
			0, nullptr,
			1, &imageMemoryBarrier);
	}

	VkQueue VulkanDevice::GetQueue() const
	{
		return m_queue;
	}


	VulkanDevice::~VulkanDevice()
	{
		if (m_validation)
			m_PFN_destroyDebugReportCallback(m_instance, m_msgCallback, nullptr);
	}
}
