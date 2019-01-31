#pragma once
#include <BaseInclude.h>
#include <../3rdParty/cereal/archives/xml.hpp>
//	所有资源类的基类.
//	资源类的基类的存在,主要是为了方便统计资源内存占用.
//	可以考虑提供内存块的类,也当作资源.这样就能管理到所有的堆内存.
//	可行的话建立起引用关系,同时如果进一步想分析原生内存情况,使用替换掉
//	malloc的方法来实现.

#include STRING_INCLUDE_PATH

namespace Core
{
	enum FileType
	{
		FileType_Scene,
		FileType_Prefab,
		FileType_GLSL_Vertex,
		FileType_GLSL_Fragment,
		FileType_RLSL_Vertex,
		FileType_RLSL_Ray,
		FileType_RLSL_Frame,
		FileType_TextureInfo,
		FileType_Texture,
		FileType_Material,
		FileType_StaticMesh,
		FileType_FBX,
	};

	class IAsset
	{
	protected:
		Bool m_beingUsed;

	public:
		ctd::string fullPathName;
		ctd::string fileName;
		ctd::string fileNameWithExt;		//	文件名,含扩展名
		FileType type;

		IAsset() : m_beingUsed(False) {}

		Bool BeingUsed()
		{
			return m_beingUsed;
		}

		virtual void BeginUse() = 0;
		virtual void Reload() = 0;
		virtual void BeforeSave() = 0;
		virtual void AfterLoad() = 0;
		virtual ~IAsset() {}
	};
}