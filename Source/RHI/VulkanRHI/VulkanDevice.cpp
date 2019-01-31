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

#include ARRAY_INCLUDE_PATH

#define VERTEX_BUFFER_BIND_ID 0

using namespace ctd;

namespace Core
{
	struct Vertex
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
		Vector3 color;
	};

	struct {
		VkPipelineVertexInputStateCreateInfo inputState;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	} vertices;

	struct {
		Matrix4x4 projection;
		Matrix4x4 model;
		Vector4 lightPos = Vector4(25.0f, 5.0f, 5.0f, 1.0f);
	} uboVS;

	VkBool32 VulkanDevice::m_messageCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData)
	{
		return VK_TRUE;
	}

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
			instanceCreateInfo.enabledLayerCount = 1;

			static const ANSICHAR *validationLayerNames[] = {
				"VK_LAYER_LUNARG_standard_validation"
			};

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

	void VulkanDevice::setupVertexDescriptions()
	{
		// Binding description
		vertices.bindingDescriptions.resize(1);
		vertices.bindingDescriptions[0] = {};
		vertices.bindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
		vertices.bindingDescriptions[0].stride = sizeof(Vertex);
		vertices.bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		// Attribute descriptions
		// Describes memory layout and shader positions
		vertices.attributeDescriptions.resize(4);
		// Location 0 : Position
		vertices.attributeDescriptions[0] = {};
		vertices.attributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
		vertices.attributeDescriptions[0].location = 0;
		vertices.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertices.attributeDescriptions[0].offset = offsetof(Vertex, pos);

		// Location 1 : Normal
		vertices.attributeDescriptions[1] = {};
		vertices.attributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
		vertices.attributeDescriptions[1].location = 1;
		vertices.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertices.attributeDescriptions[1].offset = offsetof(Vertex, normal);

		// Location 2 : Texture coordinates
		vertices.attributeDescriptions[2] = {};
		vertices.attributeDescriptions[2].binding = VERTEX_BUFFER_BIND_ID;
		vertices.attributeDescriptions[2].location = 2;
		vertices.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		vertices.attributeDescriptions[2].offset = offsetof(Vertex, uv);

		// Location 3 : Color
		vertices.attributeDescriptions[3] = {};
		vertices.attributeDescriptions[3].binding = VERTEX_BUFFER_BIND_ID;
		vertices.attributeDescriptions[3].location = 3;
		vertices.attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertices.attributeDescriptions[3].offset = offsetof(Vertex, color);

		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
		pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vertices.inputState = pipelineVertexInputStateCreateInfo;
		vertices.inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertices.bindingDescriptions.size());
		vertices.inputState.pVertexBindingDescriptions = vertices.bindingDescriptions.data();
		vertices.inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertices.attributeDescriptions.size());
		vertices.inputState.pVertexAttributeDescriptions = vertices.attributeDescriptions.data();
	}

	void VulkanDevice::setupDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
		uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uniformBufferLayoutBinding.binding = 0;
		uniformBufferLayoutBinding.descriptorCount = 1;

		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
		{
			uniformBufferLayoutBinding,
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();
		descriptorSetLayoutCreateInfo.bindingCount = setLayoutBindings.size();

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout));

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;

		VK_CHECK_RESULT(vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
	}

	void VulkanDevice::preparePipelines()
	{
		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
		pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineInputAssemblyStateCreateInfo.flags = 0;
		pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = 0;

		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
		pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		pipelineRasterizationStateCreateInfo.flags = 0;
		pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
		pipelineColorBlendAttachmentState.colorWriteMask = 0xf;
		pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.attachmentCount = 1;
		pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
		pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		pipelineDepthStencilStateCreateInfo.front = pipelineDepthStencilStateCreateInfo.back;
		pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;

		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
		pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfo.viewportCount = 1;
		pipelineViewportStateCreateInfo.scissorCount = 1;
		pipelineViewportStateCreateInfo.flags = 0;

		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
		pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineMultisampleStateCreateInfo.flags = 0;

		std::vector<VkDynamicState> dynamicStateEnables =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();
		pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateEnables.size();
		pipelineDynamicStateCreateInfo.flags = 0;

		// Solid rendering pipeline
		// Load shaders
		array<VkPipelineShaderStageCreateInfo, 2> shaderStages;

		shaderStages[0] = loadShader("mesh.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		shaderStages[1] = loadShader("mesh.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.layout = m_pipelineLayout;
		pipelineCreateInfo.renderPass = m_renderPass.Get();
		pipelineCreateInfo.flags = 0;
		pipelineCreateInfo.basePipelineIndex = -1;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

		pipelineCreateInfo.pVertexInputState = &vertices.inputState;
		pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
		pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
		pipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineCreateInfo.pStages = shaderStages.data();

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_logicalDevice, m_pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_pipeline));

		// Wire frame rendering pipeline
		//if (deviceFeatures.fillModeNonSolid) {
		//	rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
		//	rasterizationState.lineWidth = 1.0f;
		//	VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines.wireframe));
		//}
	}

	void VulkanDevice::setupDescriptorPool()
	{
		VkDescriptorPoolSize uniformBufferDescriptorPoolSize{};
		uniformBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferDescriptorPoolSize.descriptorCount = 1;

		vector<VkDescriptorPoolSize> poolSizes =
		{
			uniformBufferDescriptorPoolSize,
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
		descriptorPoolCreateInfo.maxSets = 1;

		VK_CHECK_RESULT(vkCreateDescriptorPool(m_logicalDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool));

	}

	void VulkanDevice::setupDescriptorSet()
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = &m_descriptorSetLayout;
		descriptorSetAllocateInfo.descriptorSetCount = 1;

		VK_CHECK_RESULT(vkAllocateDescriptorSets(m_logicalDevice, &descriptorSetAllocateInfo, &m_descriptorSet));

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = m_descriptorSet;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSet.dstBinding = 0;
		writeDescriptorSet.pBufferInfo = &m_pUniformBuffer.lock()->descriptorBufferInfo;
		writeDescriptorSet.descriptorCount = 1;

		vector<VkWriteDescriptorSet> writeDescriptorSets =
		{
			writeDescriptorSet,
		};

		vkUpdateDescriptorSets(m_logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
	}

	VkPipelineShaderStageCreateInfo VulkanDevice::loadShader(ANSICHAR * fileName, VkShaderStageFlagBits stage)
	{
		VkPipelineShaderStageCreateInfo shaderStage = {};
		shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage.stage = stage;

		string fileFullPathName = sceneFullPath;
		fileFullPathName += "\\";
		fileFullPathName += fileName;

		std::ifstream is(fileFullPathName.c_str(), std::ios::binary | std::ios::in | std::ios::ate);

		if (is.is_open())
		{
			size_t size = is.tellg();
			is.seekg(0, std::ios::beg);
			char* shaderCode = new char[size];
			is.read(shaderCode, size);
			is.close();

			assert(size > 0);

			VkShaderModule shaderModule;
			VkShaderModuleCreateInfo moduleCreateInfo{};
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.codeSize = size;
			moduleCreateInfo.pCode = (uint32_t*)shaderCode;

			VK_CHECK_RESULT(vkCreateShaderModule(m_logicalDevice, &moduleCreateInfo, NULL, &shaderModule));

			delete[] shaderCode;

			shaderStage.module = shaderModule;
		}
		else
		{
			assert(False);
		}

		shaderStage.pName = "main"; // todo : make param
		m_shaderModules.push_back(shaderStage.module);

		return shaderStage;
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

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = NULL;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = m_depthFormat;
		imageCreateInfo.extent = { m_width, m_height, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		imageCreateInfo.flags = 0;

		VkImageViewCreateInfo depthStencilViewCreateInfo = {};
		depthStencilViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthStencilViewCreateInfo.pNext = NULL;
		depthStencilViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthStencilViewCreateInfo.format = m_depthFormat;
		depthStencilViewCreateInfo.flags = 0;
		depthStencilViewCreateInfo.subresourceRange = {};
		depthStencilViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		depthStencilViewCreateInfo.subresourceRange.baseMipLevel = 0;
		depthStencilViewCreateInfo.subresourceRange.levelCount = 1;
		depthStencilViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		depthStencilViewCreateInfo.subresourceRange.layerCount = 1;

		VkMemoryRequirements memReqs;

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = NULL;
		memoryAllocateInfo.allocationSize = 0;
		memoryAllocateInfo.memoryTypeIndex = 0;

		VK_CHECK_RESULT(vkCreateImage(m_logicalDevice, &imageCreateInfo, nullptr, &m_depthStencilImage));
		vkGetImageMemoryRequirements(m_logicalDevice, m_depthStencilImage, &memReqs);
		memoryAllocateInfo.allocationSize = memReqs.size;
		memoryAllocateInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(m_logicalDevice, &memoryAllocateInfo, nullptr, &m_depthStencilMemory));
		VK_CHECK_RESULT(vkBindImageMemory(m_logicalDevice, m_depthStencilImage, m_depthStencilMemory, 0));

		depthStencilViewCreateInfo.image = m_depthStencilImage;

		VK_CHECK_RESULT(vkCreateImageView(m_logicalDevice, &depthStencilViewCreateInfo, nullptr, &m_depthStencilView));

		m_renderPass.Initialize(m_logicalDevice, m_swapChain.GetColorFormat(), m_depthFormat);

		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreatePipelineCache(m_logicalDevice, &pipelineCacheCreateInfo, nullptr, &m_pipelineCache));

		VkImageView attachments[2];

		attachments[1] = m_depthStencilView;

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

		setupVertexDescriptions();

		m_pUniformBuffer = m_pBufferManager->CreateBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			sizeof(uboVS));

		m_pUniformBuffer.lock()->SetDescriptorBufferInfo();

		void * mapped = m_pUniformBuffer.lock()->Map(m_logicalDevice, 0, sizeof(uboVS));

		uboVS.projection = Perspective(90.0f * Deg2Rad, (float)m_width / (float)m_height, 0.1f, 256.0f);
		uboVS.model = Translate(Matrix4x4Identify, Vector3(0.0f, 0.0f, -8.0f));

		memcpy(mapped, &uboVS, sizeof(uboVS));

		m_pUniformBuffer.lock()->Unmap(m_logicalDevice, sizeof(uboVS));

		setupDescriptorSetLayout();

		preparePipelines();

		setupDescriptorPool();
		setupDescriptorSet();
		//buildCommandBuffers();
	}

	VulkanDevice::VulkanDevice()
	{
		m_pAppName = "Vulkan Renderer";
		m_pEngineName = "Reserved";

		m_requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

		m_submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		m_validation = False;
	}

	Core::ErrorCode VulkanDevice::Initialize(void* platformHandle, void* platformWindow)
	{
		createVKInstance();

		if (m_validation)
		{
			//	TODO:	implement later.
			//m_PFN_createDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
			//m_PFN_destroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
			//m_PFN_dbgBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));
			//
			//VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
			//dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			//dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)m_messageCallback;
			//dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
			//
			//VkResult err = m_PFN_createDebugReportCallback(
			//	instance,
			//	&dbgCreateInfo,
			//	nullptr,
			//	&m_messageCallback);
		}

		uint32 gpuCount;
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_instance, &gpuCount, nullptr));

		vector<VkPhysicalDevice> physicalDevices(gpuCount);
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_instance, &gpuCount, physicalDevices.data()));
	
		m_physicalDevice = physicalDevices[0];

		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_deviceProperties);
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_deviceMemoryProperties);

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
		{
			vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
		}
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
			viewport.width = m_width;
			viewport.height = m_height;
			viewport.minDepth = 0;
			viewport.maxDepth = 1.0;

			vkCmdSetViewport(m_drawCommandBuffers[i], 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.extent.width = m_width;
			scissor.extent.height = m_height;
			scissor.offset.x = 0;
			scissor.offset.y = 0;

			vkCmdSetScissor(m_drawCommandBuffers[i], 0, 1, &scissor);

			vkCmdBindDescriptorSets(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, NULL);
			vkCmdBindPipeline(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

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

	VulkanDevice::~VulkanDevice()
	{

	}
}
