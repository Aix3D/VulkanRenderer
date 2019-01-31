#pragma once
#include <BaseInclude.h>
#include "..\Math\Vector4.h"
#include "..\Math\Vector3.h"
#include "..\Math\Vector2.h"
#include <vulkan\vulkan.h>

#include VECTOR_INCLUDE_PATH

namespace Core
{
	struct VertexP
	{
		Vector3 position;
		Vector3 normal;
		Vector2 uv0;
		Vector3 color;
	};

	struct StaticMesh
	{
	private:
		//static ctd::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
		//static ctd::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		//static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
 
	public:
		//static VkPipelineVertexInputStateCreateInfo GetVertexInputState()
		//{
			//static Bool initialized = False;
			//
			//if (!initialized)
			//{
			//	uint32 bindingID = 0;
			//
			//	// Binding description
			//	vertexInputBindingDescriptions.resize(1);
			//	vertexInputBindingDescriptions[0] = {};
			//	vertexInputBindingDescriptions[0].binding = bindingID;
			//	vertexInputBindingDescriptions[0].stride = sizeof(VertexP);
			//	vertexInputBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			//
			//	// Attribute descriptions
			//	// Describes memory layout and shader positions
			//	vertexInputAttributeDescriptions.resize(4);
			//
			//	// Location 0 : Position
			//	vertexInputAttributeDescriptions[0] = {};
			//	vertexInputAttributeDescriptions[0].binding = bindingID;
			//	vertexInputAttributeDescriptions[0].location = 0;
			//	vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			//	vertexInputAttributeDescriptions[0].offset = offsetof(VertexP, position);
			//
			//	// Location 1 : Normal
			//	vertexInputAttributeDescriptions[1] = {};
			//	vertexInputAttributeDescriptions[1].binding = bindingID;
			//	vertexInputAttributeDescriptions[1].location = 1;
			//	vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			//	vertexInputAttributeDescriptions[1].offset = offsetof(VertexP, normal);
			//
			//	// Location 2 : Texture coordinates
			//	vertexInputAttributeDescriptions[2] = {};
			//	vertexInputAttributeDescriptions[2].binding = bindingID;
			//	vertexInputAttributeDescriptions[2].location = 2;
			//	vertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			//	vertexInputAttributeDescriptions[2].offset = offsetof(VertexP, uv0);
			//
			//	// Location 3 : Color
			//	vertexInputAttributeDescriptions[3] = {};
			//	vertexInputAttributeDescriptions[3].binding = bindingID;
			//	vertexInputAttributeDescriptions[3].location = 3;
			//	vertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			//	vertexInputAttributeDescriptions[3].offset = offsetof(VertexP, color);
			//
			//	pipelineVertexInputStateCreateInfo = {};
			//	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			//	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.size());
			//	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
			//	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.size());
			//	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
			//
			//	initialized = True;
			//}

		//	return pipelineVertexInputStateCreateInfo;
		//};

		ctd::vector<VertexP> vertices;
		ctd::vector<uint32> indices;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexMemory;

		VkBuffer indexBuffer;
		VkDeviceMemory indexMemory;
	};
	
	/*
	struct StaticMesh
	{
	public:
		Vector4 * pPositions;	//	位置
		Vector3 * pNormals;		//	法线
		Vector3 * pTangents;	//	切线
		Vector3 * pBinormals;	//	副法线
		Vector4 * pColors;		//	颜色
		Vector2 * pUV0s;		//	UV0
		Vector2 * pUV1s;		//	UV1
		Vector2 * pUV2s;		//	UV2
		Vector2 * pUV3s;		//	UV3
		Vector2 * pUV4s;		//	UV4
		Vector2 * pUV5s;		//	UV5
		Vector2 * pUV6s;		//	UV6
		Vector2 * pUV7s;		//	UV7

		uint32 * pIndices;		//	索引

		int32 vertexCount;
		int32 indexCount;
	};
	*/
}