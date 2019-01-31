//#include "VulkanShaderModule.h"
//
//namespace Core
//{
//	VulkanShaderModule::VulkanShaderModule()
//	{
//	}
//
//	VkPipelineShaderStageCreateInfo VulkanShaderModule::Create(uint32 * pShaderCode, VkShaderStageFlagBits stageFlagBits)
//	{
//		VkPipelineShaderStageCreateInfo shaderStage = {};
//		shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		shaderStage.stage = stageFlagBits;
//
//		string fileFullPathName = sceneFullPath;
//		fileFullPathName += "\\";
//		fileFullPathName += fileName;
//
//		std::ifstream is(fileFullPathName.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
//
//		if (is.is_open())
//		{
//			size_t size = is.tellg();
//			is.seekg(0, std::ios::beg);
//			char* shaderCode = new char[size];
//			is.read(shaderCode, size);
//			is.close();
//
//			assert(size > 0);
//
//			VkShaderModule shaderModule;
//			VkShaderModuleCreateInfo moduleCreateInfo{};
//			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//			moduleCreateInfo.codeSize = size;
//			moduleCreateInfo.pCode = (uint32_t*)shaderCode;
//
//			VK_CHECK_RESULT(vkCreateShaderModule(m_logicalDevice, &moduleCreateInfo, NULL, &shaderModule));
//
//			delete[] shaderCode;
//
//			shaderStage.module = shaderModule;
//		}
//		else
//		{
//			assert(False);
//		}
//
//		shaderStage.pName = "main"; // todo : make param
//		m_shaderModules.push_back(shaderStage.module);
//
//		return shaderStage;
//	}
//
//	VulkanShaderModule::~VulkanShaderModule()
//	{
//
//	}
//}