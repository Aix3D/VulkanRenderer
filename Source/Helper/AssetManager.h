#pragma once
#include <BaseInclude.h>
#include <vulkan\vulkan.h>
#include "..\Math\Vector3.h"
#include "..\Math\Vector2.h"
#include "..\Component\StaticMesh.h"
#include "..\RHI\VulkanRHI\VulkanDevice.h"
#include "..\Component\RawShader.h"

#include STRING_INCLUDE_PATH
#include MAP_INCLUDE_PATH

namespace Core
{
	class AssetManager
	{
	private:
		const ANSICHAR * fbxFullPath = "G:\\Scenes";// "G:\\Projects\\Template0\\Assets\\Scenes";
		const ANSICHAR * shaderFullPath = "G:\\Scenes";

		ctd::map<ctd::string, std::shared_ptr<StaticMesh>> staitcMeshMap;
		ctd::map<ctd::string, std::shared_ptr<RawShader>> shaderMap;

	public:
		AssetManager();
		void LoadFBX(ANSICHAR * fbxName);
		std::shared_ptr<RawShader> LoadRawShader(ANSICHAR * shaderName);
		void UploadToGPU(VulkanDevice * pDevice);
		void CommitToDrawList(VulkanDevice * pDevice);
		~AssetManager();
	};
}