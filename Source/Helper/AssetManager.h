#pragma once
#include <BaseInclude.h>
#include <vulkan\vulkan.h>
#include "..\Math\Vector3.h"
#include "..\Math\Vector2.h"
#include "..\Component\StaticMesh.h"
#include "..\RHI\VulkanRHI\VulkanDevice.h"
#include "..\Component\RawShader.h"
#include <gli/gli.hpp>
#include "..\Component\Material.h"
#include "..\Asset\VulkanTexture.h"

#include STRING_INCLUDE_PATH
#include MAP_INCLUDE_PATH

namespace Core
{
	class AssetManager
	{
	private:
		const ANSICHAR * fbxFullPath = "G:\\Scenes";// "G:\\Projects\\Template0\\Assets\\Scenes";
		const ANSICHAR * shaderFullPath = "G:\\Scenes";
		const ANSICHAR * textureFullPath = "G:\\Scenes";

		ctd::map<ctd::string, std::shared_ptr<StaticMesh>> staitcMeshMap;
		ctd::map<ctd::string, std::shared_ptr<Material>> materialMap;
		ctd::map<ctd::string, std::shared_ptr<RawShader>> shaderMap;
		ctd::map<ctd::string, std::shared_ptr<VulkanTexture>> textureMap;

	public:
		AssetManager();
		void LoadFBX(ANSICHAR * fbxName);
		std::shared_ptr<RawShader> LoadRawShader(ANSICHAR * shaderName);
		std::shared_ptr<VulkanTexture> LoadTexture(ANSICHAR * textureName);
		void UploadToGPU(VulkanDevice * pDevice);
		void UploadTexture(VulkanDevice * pDevice);
		void CommitToDrawList(VulkanDevice * pDevice);
		std::shared_ptr<VulkanTexture> GetAnyTexture();
		~AssetManager();
	};
}