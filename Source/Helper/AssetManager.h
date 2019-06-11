#pragma once
#include <BaseInclude.h>
#include <vulkan\vulkan.h>
#include <gli/gli.hpp>
#include "..\RHI\VulkanRHI\VulkanDevice.h"
#include "..\Math\Vector3.h"
#include "..\Math\Vector2.h"
#include "..\Asset\RawTexture.h"
#include "..\Asset\RawShader.h"
#include "..\Asset\RawFBX.h"
#include "..\Asset\Texture.h"
#include "..\Asset\Shader.h"
#include "..\Asset\StaticMesh.h"
#include "..\Asset\Material.h"
#include "..\Asset\Entity.h"
#include "..\Asset\World.h"

#include STRING_INCLUDE_PATH
#include MAP_INCLUDE_PATH

namespace Core
{
	class AssetManager
	{
	private:
		const ANSICHAR * rawTextureFullPath = "E:\\Projects\\Template0\\Assets\\Textures";
		const ANSICHAR * rawShaderFullPath = "E:\\Projects\\Template0\\Assets\\Shaders\\GLSL";
		const ANSICHAR * rawFbxFullPath = "E:\\Projects\\Template0\\Assets\\FBXs";

		const ANSICHAR * defaultVertexShaderName = "mesh";
		const ANSICHAR * defaultFragmentShaderName = "mesh";

		ctd::map<ctd::string, std::shared_ptr<RawTexture>> rawTextureMap;
		ctd::map<ctd::string, std::shared_ptr<RawShader>> rawShaderMap;
		ctd::map<ctd::string, std::shared_ptr<RawFBX>> rawFbxMap;

		ctd::map<ctd::string, std::shared_ptr<Texture>> textureMap;
		ctd::map<ctd::string, std::shared_ptr<Shader>> vertexShaderMap;
		ctd::map<ctd::string, std::shared_ptr<Shader>> fragmentShaderMap;
		ctd::map<ctd::string, std::shared_ptr<Material>> materialMap;
		ctd::map<ctd::string, std::shared_ptr<StaticMesh>> staitcMeshMap;
		ctd::map<ctd::string, std::shared_ptr<World>> worldMap;

		void loadRawTexture(const ANSICHAR * textureName, const ANSICHAR * fileFullPathName);
		void loadRawShader(const ANSICHAR * fileFullPathName, Shader * pOutShader);
		void loadRawFBX(const ANSICHAR * fbxName, const ANSICHAR * fileFullPathName);

		void processRawTexture(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName);
		void processRawShader(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName);
		void processRawFBX(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName);
		void processFolder(AssetType assetType, const ANSICHAR * folderFullPathName, const ANSICHAR * extension0, ...);

	public:
		const ANSICHAR * fbxExt = ".fbx";
		const ANSICHAR * vertexShaderExt = ".vert_spv";
		const ANSICHAR * geometryShaderExt = ".geo_spv";
		const ANSICHAR * fragmentShaderExt = ".frag_spv";
		const ANSICHAR * textureKTXExt = ".ktx";
		const ANSICHAR * texturePNGExt = ".png";
		const ANSICHAR * dummyTextureName = "dummy";
		const ANSICHAR * defaultWorldName = "default";
		const ANSICHAR * defaultCameraName = "camera";

		AssetManager();
		void Scan();
		std::shared_ptr<World> GetDefaultWorld();
		void UploadToGPU(VulkanDevice * pDevice);
		void UploadTexture(VulkanDevice * pDevice);
		void CommitToDrawList(VulkanDevice * pDevice);
		~AssetManager();
	};
}