#include "AssetManager.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <fstream>
#include <io.h>
#include <MathGlobal.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include VECTOR_INCLUDE_PATH

using namespace ctd;

namespace Core
{
	void AssetManager::loadRawTexture(const ANSICHAR * textureName, const ANSICHAR * fileFullPathName)
	{
		textureMap[textureName] = std::make_shared<Texture>();

		int32 width;
		int32 height;
		int32 channelCount;

		uint8 * pData = stbi_load(fileFullPathName, &width, &height, &channelCount, 0);

		textureMap[textureName]->width = width;
		textureMap[textureName]->height = height;
		textureMap[textureName]->imageDataSize = width * height * channelCount;
		textureMap[textureName]->mipLevels = 1;

		textureMap[textureName]->pImage = std::make_unique<uint8[]>(textureMap[textureName]->imageDataSize);

		memcpy(textureMap[textureName]->pImage.get(), pData, textureMap[textureName]->imageDataSize);

		stbi_image_free(pData);

		//if (pRawSurface)
		//{
		//	SDL_PixelFormat * pFormat = pRawSurface->format;
		//
		//	textureMap[textureName]->width = pRawSurface->w;
		//	textureMap[textureName]->height = pRawSurface->h;
		//
		//	Uint32 pixelFormat = SDL_PIXELFORMAT_UNKNOWN;
		//
		//	if (pRawSurface->format->BytesPerPixel == 3)
		//		pixelFormat = SDL_PIXELFORMAT_RGB24;
		//	else if (pRawSurface->format->BytesPerPixel == 4)
		//		pixelFormat = SDL_PIXELFORMAT_RGBA32;
		//
		//	SDL_Surface* pRGBSurface = SDL_ConvertSurfaceFormat(pRawSurface, pixelFormat, 0);
		//
		//	SDL_FreeSurface(pRawSurface);
		//
		//	int32 imageSize = pRawSurface->w * pRawSurface->h * sizeof(uint8) * pRGBSurface->format->BytesPerPixel;
		//	textureMap[textureName]->imageDataSize = imageSize;
		//
		//	textureMap[textureName]->pImage = std::make_unique<ANSICHAR[]>(imageSize);
		//
		//	memcpy(textureMap[textureName]->pImage.get(),
		//		pRGBSurface->pixels,
		//		imageSize);
		//
		//	SDL_FreeSurface(pRGBSurface);
		//}
	}

	void AssetManager::loadRawShader(const ANSICHAR * fileFullPathName, Shader * pOutShader)
	{
		std::ifstream is(fileFullPathName, std::ios::binary | std::ios::in | std::ios::ate);

		if (is.is_open())
		{
			size_t size = is.tellg();
			is.seekg(0, std::ios::beg);
			pOutShader->pRawData = std::make_unique<ANSICHAR[]>(size);
			pOutShader->size = size;
			is.read(reinterpret_cast<char *>(pOutShader->pRawData.get()), size);
			is.close();
		}
	}

	void AssetManager::loadRawFBX(const ANSICHAR * fbxName, const ANSICHAR * fileFullPathName)
	{
		const aiScene* scene;
		Assimp::Importer Importer;

		static const int assimpFlags =
			aiProcess_FlipWindingOrder |
			aiProcess_Triangulate |
			aiProcess_PreTransformVertices;

		scene = Importer.ReadFile(fileFullPathName, assimpFlags);

		if (scene == Null)
			return;

		ctd::vector<std::shared_ptr<StaticMesh>> staticMeshes;
		ctd::vector<std::shared_ptr<Material>> materials;

		ctd::map<int32, int32> duplicatedMaterialMap;
		ctd::vector<ctd::string> currentFbxMaterialNames;

		for (uint32 materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
		{
			//	Processing Material
			aiMaterial * currentMaterial = scene->mMaterials[materialIndex];

			aiString naiveMaterialName;
			currentMaterial->Get(AI_MATKEY_NAME, naiveMaterialName);
			ctd::string materialName = fbxName;
			materialName += "_";
			materialName += naiveMaterialName.C_Str();

			ctd::map<ctd::string, std::shared_ptr<Material>>::iterator iter = materialMap.find(materialName.c_str());

			if (iter == materialMap.end())
			{
				materialMap[materialName] = std::make_shared<Material>();
				materialMap[materialName]->name = materialName;

				materials.push_back(materialMap[materialName]);

				currentFbxMaterialNames.push_back(materialName);

				aiColor4D albedoColor;
				currentMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, albedoColor);
				materialMap[materialName]->albedoColor[0] = albedoColor.r;
				materialMap[materialName]->albedoColor[1] = albedoColor.g;
				materialMap[materialName]->albedoColor[2] = albedoColor.b;
				materialMap[materialName]->albedoColor[3] = albedoColor.a;

				currentMaterial->Get(AI_MATKEY_OPACITY, materialMap[materialName]->opacity);

				//	XXX:
				//		临时代码,测试alpha blending
				//if (materialName == "baiqiang")
				//	materialMap[materialName]->albedoColor[3] = materialMap[materialName]->opacity;

				//  XXX:
				//      暂时没有定下来什么材质属性来标记透明,
				//      所以不处理透明.
				ctd::string vertexShaderName = defaultVertexShaderName;
				ctd::string fragmentShaderName = defaultFragmentShaderName;

				materialMap[materialName]->vertexShader = vertexShaderMap[vertexShaderName];
				materialMap[materialName]->fragmentShader = fragmentShaderMap[fragmentShaderName];

				// Diffuse
				//if (currentMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
				//{
				//	aiString rawTextureName;
				//
				//	currentMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &rawTextureName);
				//
				//	ctd::string textureNameWithExt = rawTextureName.C_Str();
				//
				//	//	XXX:
				//	//		platform independent slashes
				//	size_t offset = textureNameWithExt.find_last_of('\\');
				//
				//	textureNameWithExt = textureNameWithExt.substr(offset + 1, textureNameWithExt.length() - offset);
				//
				//	offset = textureNameWithExt.find_last_of('.');
				//
				//	ctd::string textureName = textureNameWithExt.substr(0, offset);
				//
				//	ctd::string textureExt = textureNameWithExt.substr(offset, textureNameWithExt.length() - offset);
				//
				//	if (textureExt != texturePNGExt)
				//		assert(False);
				//
				//	materialMap[materialName]->albedoTexture = textureMap[textureName];
				//}
				//else
				{
					//	assign dummy texture to the material which without albedo texture
					materialMap[materialName]->albedoTexture = textureMap[dummyTextureName];
				}
			}
			else
			{
				//	duplicated material from fbx file processing
				for (int32 i = 0; i < currentFbxMaterialNames.size(); ++i)
				{
					if (currentFbxMaterialNames[i] == materialName)
						duplicatedMaterialMap[materialIndex] = i;
				}
			}
		}

		for (uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			aiMesh* currentMesh = scene->mMeshes[meshIndex];

			//	Processing Mesh
			string meshName = fbxName;
			meshName += "_" + std::to_string(meshIndex);

			if (staitcMeshMap.find(meshName) == staitcMeshMap.end())
			{
				staitcMeshMap[meshName] = std::make_shared<StaticMesh>();

				staticMeshes.push_back(staitcMeshMap[meshName]);

				for (uint32 vertexIndex = 0; vertexIndex < currentMesh->mNumVertices; ++vertexIndex)
				{
					Vertex vertex;

					vertex.position = Vector3(currentMesh->mVertices[vertexIndex].x, currentMesh->mVertices[vertexIndex].y, currentMesh->mVertices[vertexIndex].z);
					vertex.normal = Vector3(currentMesh->mNormals[vertexIndex].x, currentMesh->mNormals[vertexIndex].y, currentMesh->mNormals[vertexIndex].z);
					vertex.uv0 = Vector2(currentMesh->mTextureCoords[0][vertexIndex].x, currentMesh->mTextureCoords[0][vertexIndex].y);
					vertex.color = Vector3(1.0, 1.0, 1.0);

					staitcMeshMap[meshName]->vertices.push_back(vertex);
				}

				ctd::map<int32, int32>::iterator iter = duplicatedMaterialMap.find(currentMesh->mMaterialIndex);

				if (iter != duplicatedMaterialMap.end())
					staitcMeshMap[meshName]->materialIndex = iter->second;
				else
					staitcMeshMap[meshName]->materialIndex = currentMesh->mMaterialIndex;

				for (uint32 faceIndex = 0; faceIndex < currentMesh->mNumFaces; ++faceIndex)
				{
					for (uint32 triangleIndex = 0; triangleIndex < 3; ++triangleIndex)
					{
						staitcMeshMap[meshName]->indices.push_back(currentMesh->mFaces[faceIndex].mIndices[triangleIndex]);
					}
				}
			}
		}

		if (worldMap.find(defaultWorldName) == worldMap.end())
		{
			worldMap[defaultWorldName] = std::make_shared<World>();

			worldMap[defaultWorldName]->CreateCamera() ;
		}

		//	Add resources from current fbx to the default world,
		//	it might be the editor world 
		for (size_t i = 0; i < staticMeshes.size(); ++i)
		{
			worldMap[defaultWorldName]->CreateRenderableEntity(staticMeshes[i], materials[staticMeshes[i]->materialIndex]);
		}
	}

	void AssetManager::processRawTexture(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<RawTexture> rawTextureRef = rawTextureMap[fileName];

		if (rawTextureRef.expired())
		{
			rawTextureMap[fileName] = make_shared<RawTexture>();
			rawTextureRef = rawTextureMap[fileName];
		}

		rawTextureRef.lock()->fullPathName = fileFullPathName;
		rawTextureRef.lock()->fileName = fileName;
		rawTextureRef.lock()->fileNameWithExt = fileName;
		rawTextureRef.lock()->fileNameWithExt += extension;
		rawTextureRef.lock()->type = AssetType_RawTexture;

		loadRawTexture(fileName, fileFullPathName);
	}

	void AssetManager::processRawShader(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<RawShader> rawShaderRef = rawShaderMap[fileName];

		if (rawShaderRef.expired())
		{
			rawShaderMap[fileName] = make_shared<RawShader>();
			rawShaderRef = rawShaderMap[fileName];
		}

		rawShaderRef.lock()->fullPathName = fileFullPathName;
		rawShaderRef.lock()->fileName = fileName;
		rawShaderRef.lock()->fileNameWithExt = fileName;
		rawShaderRef.lock()->fileNameWithExt += extension;
		rawShaderRef.lock()->type = AssetType_RawShader;

		weak_ptr<Shader> shaderRef;

		if (strcmp(extension, vertexShaderExt) == 0)
		{
			shaderRef = vertexShaderMap[fileName];

			if (shaderRef.expired())
				vertexShaderMap[fileName] = make_shared<Shader>();

			shaderRef = vertexShaderMap[fileName];
			shaderRef.lock()->type = ShaderType_Vertex;
		}
		else if (strcmp(extension, geometryShaderExt) == 0)
		{
		}
		else if (strcmp(extension, fragmentShaderExt) == 0)
		{
			shaderRef = fragmentShaderMap[fileName];

			if (shaderRef.expired())
				fragmentShaderMap[fileName] = make_shared<Shader>();

			shaderRef = fragmentShaderMap[fileName];
			shaderRef.lock()->type = ShaderType_Fragment;
		}

		loadRawShader(fileFullPathName, shaderRef.lock().get());
	}

	void AssetManager::processRawFBX(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<RawFBX> fbxRef = rawFbxMap[fileName];

		if (fbxRef.expired())
		{
			rawFbxMap[fileName] = make_shared<RawFBX>();
			fbxRef = rawFbxMap[fileName];
		}

		fbxRef.lock()->fullPathName = fileFullPathName;
		fbxRef.lock()->fileName = fileName;
		fbxRef.lock()->fileNameWithExt = fileName;
		fbxRef.lock()->fileNameWithExt += extension;
		fbxRef.lock()->type = AssetType_RawFBX;

		loadRawFBX(fileFullPathName, fileFullPathName);
	}

	void AssetManager::processFolder(AssetType assetType, const ANSICHAR * folderFullPathName, const ANSICHAR * extension0, ...)
	{
		string pattern(folderFullPathName);
		pattern.append("\\*");

		WIN32_FIND_DATA data;

		HANDLE hFind;

		if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				string fileName = data.cFileName;

				size_t pos = fileName.find_last_of('.');

				string extension = fileName.substr(pos, fileName.length() - 1);
				transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

				string fileNameWithoutExt = fileName.substr(0, pos);

				va_list args;
				va_start(args, extension0);
				const ANSICHAR * extensionCursor;

				extensionCursor = extension0;

				while (extensionCursor)
				{
					if (extension == extensionCursor)
					{
						string fullPathName = folderFullPathName;
						fullPathName.append("\\" + fileName);

						switch (assetType)
						{
						case AssetType_RawTexture:
							processRawTexture(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_Shader:
							processRawShader(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_RawFBX:
							processRawFBX(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						}

						break;
					}

					extensionCursor = va_arg(args, const ANSICHAR *);
				}

				va_end(args);
			} while (FindNextFile(hFind, &data) != 0);

			FindClose(hFind);
		}
	}

	AssetManager::AssetManager()
	{
	}

	void AssetManager::Scan()
	{
		//	TODO:	添加新平台编辑器支持时再更改此方法.哪年哪月?
		//			2018/4/11-15:30 北京七棵树创意园b2-2北门.
#ifdef PLATFORM_WINDOWS
		processFolder(AssetType_RawTexture, rawTextureFullPath, texturePNGExt, Null);
		processFolder(AssetType_Shader, rawShaderFullPath, vertexShaderExt, geometryShaderExt, fragmentShaderExt, Null);
		processFolder(AssetType_RawFBX, rawFbxFullPath, fbxExt, Null);
#endif
	}

	std::shared_ptr<Core::World> AssetManager::GetDefaultWorld() 
	{
		return worldMap[defaultWorldName];
	}

	void AssetManager::UploadToGPU(VulkanDevice * pDevice)
	{
		for (map<ctd::string, std::shared_ptr<StaticMesh>>::iterator iter = staitcMeshMap.begin();
			iter != staitcMeshMap.end();
			++iter)
		{
			pDevice->CreateBuffer(
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
					iter->second->vertices.size() * sizeof(Vertex),
					&iter->second->vertexBuffer,
					&iter->second->vertexMemory,
					iter->second->vertices.data());

			pDevice->CreateBuffer(
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				iter->second->indices.size() * sizeof(uint32),
				&iter->second->indexBuffer,
				&iter->second->indexMemory,
				iter->second->indices.data());
		}

		for (map<std::string, std::shared_ptr<Texture>>::iterator iter = textureMap.begin();
			iter != textureMap.end();
			++iter)
		{
			iter->second->UploadToGPU(pDevice);
		}
	}

	void AssetManager::UploadTexture(VulkanDevice * pDevice)
	{
		for (map<ctd::string, std::shared_ptr<Texture>>::iterator iter = textureMap.begin();
			iter != textureMap.end();
			++iter)
		{
			iter->second->UploadToGPU(pDevice);
		}
	}

	void AssetManager::CommitToDrawList(VulkanDevice * pDevice)
	{
		for (map<ctd::string, std::shared_ptr<StaticMesh>>::iterator iter = staitcMeshMap.begin();
			iter != staitcMeshMap.end();
			++iter)
		{
			VkDeviceSize offsets[1] = { 0 };

			BindVertexBuffers * bindVertexBuffers = new BindVertexBuffers(
				0,
				1,
				&iter->second->vertexBuffer,
				offsets);

			BindIndexBuffer * bindIndexBuffer = new BindIndexBuffer(
				iter->second->indexBuffer,
				0,
				VK_INDEX_TYPE_UINT32);
			
			DrawIndexed * drawIndexed = new DrawIndexed(
				static_cast<uint32>(iter->second->indices.size()),
				1,
				0,
				0,
				0);

			pDevice->AddCommand(bindVertexBuffers);
			pDevice->AddCommand(bindIndexBuffer);
			pDevice->AddCommand(drawIndexed);
		}
	}

	AssetManager::~AssetManager()
	{

	}
}