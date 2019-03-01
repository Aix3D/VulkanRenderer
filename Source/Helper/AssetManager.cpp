#include "AssetManager.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <fstream>
#include <io.h>
#include <MathGlobal.h>

#include VECTOR_INCLUDE_PATH

using namespace ctd;

namespace Core
{
	AssetManager::AssetManager()
	{

	}

	void AssetManager::LoadFBX(ANSICHAR * fbxName)
	{
		const aiScene* scene;
		Assimp::Importer Importer;

		static const int assimpFlags =
			aiProcess_FlipWindingOrder |
			aiProcess_Triangulate |
			aiProcess_PreTransformVertices;

		ctd::string fileFullPathName = fbxFullPath;
		fileFullPathName += "\\";
		fileFullPathName += fbxName;

		scene = Importer.ReadFile(fileFullPathName.c_str(), assimpFlags);

		for (uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			aiMesh* currentMesh = scene->mMeshes[meshIndex];

			string meshName = std::to_string(meshIndex) + scene->mRootNode->mName.C_Str();

			staitcMeshMap[meshName] = std::make_shared<StaticMesh>();
			
			for (uint32 vertexIndex = 0; vertexIndex < currentMesh->mNumVertices; ++vertexIndex)
			{
				VertexP vertex;

				vertex.position = Vector3(currentMesh->mVertices[vertexIndex].x, currentMesh->mVertices[vertexIndex].y, currentMesh->mVertices[vertexIndex].z);
				vertex.normal = Vector3(currentMesh->mNormals[vertexIndex].x, currentMesh->mNormals[vertexIndex].y, currentMesh->mNormals[vertexIndex].z);
				vertex.uv0 = Vector2(currentMesh->mTextureCoords[0][vertexIndex].x, currentMesh->mTextureCoords[0][vertexIndex].y);
				vertex.color = Vector3(1.0, 1.0, 1.0);

				staitcMeshMap[meshName]->vertices.push_back(vertex);
			}

			for (uint32 faceIndex = 0; faceIndex < currentMesh->mNumFaces; ++faceIndex)
			{
				for (uint32 triangleIndex = 0; triangleIndex < 3; ++triangleIndex)
				{
					staitcMeshMap[meshName]->indices.push_back(currentMesh->mFaces[faceIndex].mIndices[triangleIndex]);
				}
			}

			aiMaterial * currentMaterial = scene->mMaterials[meshIndex];

			aiString materialName;
			currentMaterial->Get(AI_MATKEY_NAME, materialName);

			std::shared_ptr<Material> material = std::make_shared<Material>();
			materialMap[materialName.C_Str()] = material;

			aiColor4D color;
			currentMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
			material->properites.ambient = Vector4(color.r, color.g, color.b, color.a);
			currentMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			material->properites.diffuse = Vector4(color.r, color.g, color.b, color.a);
			currentMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
			material->properites.specular = Vector4(color.r, color.g, color.b, color.a);
			currentMaterial->Get(AI_MATKEY_OPACITY, material->properites.opacity);

			if ((material->properites.opacity) > 0.0f)
				material->properites.specular = Vector4Dummy ;

			// Diffuse
			if (currentMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString textureName;

				currentMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);
				string fileName = string(textureName.C_Str());
				replace(fileName.begin(), fileName.end(), '\\', '/');
				fileName.insert(fileName.find(".ktx"), "_bc3_unorm");
				LoadTexture(const_cast<ANSICHAR*>(fileName.c_str()));
			}
		}
	}

	std::shared_ptr<RawShader> AssetManager::LoadRawShader(ANSICHAR * shaderName)
	{
		string fileFullPathName = shaderFullPath;
		fileFullPathName += "\\";
		fileFullPathName += shaderName;

		std::ifstream is(fileFullPathName.c_str(), std::ios::binary | std::ios::in | std::ios::ate);

		if (is.is_open())
		{
			size_t size = is.tellg();
			is.seekg(0, std::ios::beg);
			ANSICHAR * shaderCode = new ANSICHAR[size];
			is.read(shaderCode, size);
			is.close();

			shaderMap[shaderName] = std::make_shared<RawShader>();
			shaderMap[shaderName]->pRawData = std::make_unique<ANSICHAR*>(shaderCode);
			shaderMap[shaderName]->size = size;

			return shaderMap[shaderName];
		}

		return Null;
	}

	std::shared_ptr<VulkanTexture> AssetManager::LoadTexture(ANSICHAR * textureName)
	{
		string fileFullPathName = textureFullPath;
		fileFullPathName += "\\";
		fileFullPathName += textureName;

		gli::texture2d tex2D(gli::load(fileFullPathName));

		textureMap[textureName] = std::make_shared<VulkanTexture>();
		textureMap[textureName]->rawTexture = tex2D;
	
		return textureMap[textureName];
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
					iter->second->vertices.size() * sizeof(VertexP),
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

		for (map<std::string, std::shared_ptr<VulkanTexture>>::iterator iter = textureMap.begin();
			iter != textureMap.end();
			++iter)
		{
			iter->second->UploadToGPU(pDevice);
		}
	}

	void AssetManager::UploadTexture(VulkanDevice * pDevice)
	{
		for (map<ctd::string, std::shared_ptr<VulkanTexture>>::iterator iter = textureMap.begin();
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
				iter->second->indices.size(),
				1,
				0,
				0,
				0);

			pDevice->AddCommand(bindVertexBuffers);
			pDevice->AddCommand(bindIndexBuffer);
			pDevice->AddCommand(drawIndexed);
		}
	}

	std::shared_ptr<Core::VulkanTexture> AssetManager::GetAnyTexture()
	{
		return textureMap["KAMEN-stup_bc3_unorm.ktx"];
	}

	AssetManager::~AssetManager()
	{

	}

}