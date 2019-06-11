#pragma once
#include "IAsset.h"
#include "..\Math\Vector4.h"
#include "..\Math\Vector3.h"
#include "..\Math\Vector2.h"
#include <vulkan\vulkan.h>

#include VECTOR_INCLUDE_PATH

namespace Core
{
	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector2 uv0;
		Vector3 color;
	};

	class StaticMesh : public IAsset
	{
	public:
		ctd::vector<Vertex> vertices;
		ctd::vector<uint32> indices;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexMemory;

		VkBuffer indexBuffer;
		VkDeviceMemory indexMemory;

		//	XXX:
		//		������ȡ��Դ��ʱ���õ�,�о�����ʱ�����Ż���.
		uint32 materialIndex;

		StaticMesh();
		int32 GetFeatureID() const;
		virtual void OnPause() override;
		virtual void OnResume() override;
		virtual ~StaticMesh();
	};
}