#pragma once
#include <BaseInclude.h>
#include "../3rdParty/FBXSDK/fbxsdk.h"
#include "../3rdParty/FBXSDK/fbxsdk/core/fbxmanager.h"
#include "../Asset/StaticMesh.h"
#include "../DesignPattern/SingletonPattern/Singleton.h"
#include "../Asset/Material.h"

namespace Core
{
	class FBXImportor : public Singleton<FBXImportor>
	{
	private:
		ErrorCode initializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
		ErrorCode loadScene(FbxManager* pManager, FbxDocument* pScene, const ANSICHAR* pFilename);
		void processMesh(FbxNode* pNode, StaticMesh * pOutStaticMesh, Bool generateTangentsData = True);
		void processMesh(FbxNode* pNode, ctd::vector<std::unique_ptr<StaticMesh>> & outStaticMeshes, ctd::vector<std::unique_ptr<Material>> & outMaterials, Bool generateTangentsData = True);
		void processNode(FbxNode* pNode, StaticMesh * pOutStaticMesh, Bool generateTangentsData = True);
		void processNode(FbxNode* pNode, ctd::vector<std::unique_ptr<StaticMesh>> & outStaticMeshes, ctd::vector<std::unique_ptr<Material>> & outMaterials, Bool generateTangentsData = True);
	public:
		ErrorCode LoadMesh(const ANSICHAR * fileFullPathName, StaticMesh * pOutStaticMesh, Bool generateTangentsData = True);
		ErrorCode LoadFBX(const ANSICHAR * fileFullPathName, ctd::vector<std::unique_ptr<StaticMesh>> & outStaticMeshes, ctd::vector<std::unique_ptr<Material>> & outMaterials, Bool generateTangentsData = True);
	};
}