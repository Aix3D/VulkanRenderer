#include "StaticMesh.h"
#include "..\Helper\MeshImportor.h"
#include "..\Math\MathGlobal.h"
#include "../3rdParty/UVAtlas/UVAtlas.h"
#include "..\3rdParty\DirectXMesh\DirectXMesh.h"
#include "..\Settings\LightmappingSetting.h"
#include "../3rdParty/TheklaAtlas/thekla_atlas.h"
#include "..\3rdParty\RectangleBinPack\MaxRectsBinPack.h"
#include "..\Helper\FBXImportor.h"

using namespace DirectX;
using namespace Thekla;
using namespace ctd;

namespace Core
{
	float StaticMesh::getArea(const Vector3 & v0, const Vector3 & v1, const Vector3 & v2)
	{
		Vector3 crossValue = Cross(v1 - v0, v2 - v0);

		return abs(crossValue.length()) * 0.5f;
	}

	float StaticMesh::getArea(const Vector2 & v0, const Vector2 & v1, const Vector2 & v2)
	{
		return abs(Cross(v1 - v0, v2 - v0)) * 0.5f;
	}

	float StaticMesh::getTriangleArea(const Vector3 & v0, const Vector3 & v1, const Vector3 & v2)
	{
		Vector3 crossValue = Cross(v1 - v0, v2 - v0);

		return abs(crossValue.length()) * 0.5f;
	}

	float StaticMesh::getTriangleArea(const Vector2 & v0, const Vector2 & v1, const Vector2 & v2)
	{
		Vector2 v0Temp = v0 - v2;
		Vector2 v1Temp = v1 - v2;

		return (v0Temp.x * v1Temp.y - v0Temp.y * v1Temp.x) * 0.5f;
	}

	void StaticMesh::generateUV1UVAtlas()
	{
		XMFLOAT3 * pPosition3 = new XMFLOAT3[vertexCount];

		for (int32 i = 0; i < vertexCount; ++i)
		{
			pPosition3[i].x = pPositions[i].x;
			pPosition3[i].y = pPositions[i].y;
			pPosition3[i].z = pPositions[i].z;
		}

		float * pIMTData = new float[indexCount];

		HRESULT result = DirectX::UVAtlasComputeIMTFromPerVertexSignal(
			pPosition3,
			vertexCount,
			pIndices,
			DXGI_FORMAT_R32_UINT,
			indexCount / 3,
			reinterpret_cast<float *>(pNormals),
			3,
			sizeof(Vector3),
			Null,
			pIMTData);

		uint32 * pAdjacency = new uint32[indexCount];

		result = GenerateAdjacencyAndPointReps(
			pIndices,
			indexCount / 3,
			pPosition3,
			vertexCount,
			0.01f,
			Null,
			pAdjacency);

		ctd::vector<UVAtlasVertex> vb;
		ctd::vector<uint8_t> ib;
		ctd::vector<uint32_t> facePartitioning;
		ctd::vector<uint32_t> vertexRemapArray;
		ctd::vector<uint32_t> partitionResultAdjacency;
		float outStretch = 0.f;
		size_t outCharts = 0;

		result = UVAtlasPartition(
			pPosition3,
			vertexCount,
			pIndices,
			DXGI_FORMAT_R32_UINT,
			indexCount / 3,
			LightmappingSetting::Instance()->maxChartNumber,
			LightmappingSetting::Instance()->maxStrechSize,
			pAdjacency,
			Null,
			pIMTData,
			Null,
			0,
			UVATLAS_GEODESIC_FAST,
			vb,
			ib,
			&facePartitioning,
			&vertexRemapArray,
			partitionResultAdjacency,
			&outStretch,
			&outCharts);

		ctd::vector<uint32> * pIndices = reinterpret_cast<ctd::vector<uint32> *>(&ib);

		float positionAreaTotal = 0;
		float uvAreaTotal = 0;

		for (int32 triangleIndex = 0; triangleIndex < indexCount / 3; ++triangleIndex)
		{
			uint32 v0Index = (*pIndices)[triangleIndex * 3];
			uint32 v1Index = (*pIndices)[triangleIndex * 3 + 1];
			uint32 v2Index = (*pIndices)[triangleIndex * 3 + 2];

			float positionArea = getArea(
				reinterpret_cast<Vector3 &>(vb[v0Index].pos),
				reinterpret_cast<Vector3 &>(vb[v1Index].pos),
				reinterpret_cast<Vector3 &>(vb[v2Index].pos));

			positionAreaTotal += positionArea;

			float uvArea = getArea(
				reinterpret_cast<Vector2 &>(vb[v0Index].uv),
				reinterpret_cast<Vector2 &>(vb[v1Index].uv),
				reinterpret_cast<Vector2 &>(vb[v2Index].uv));

			uvAreaTotal += uvArea;
		}

		float division = uvAreaTotal / positionAreaTotal ;
		float scale = LightmappingSetting::Instance()->uvScale / division;

		rbp::MaxRectsBinPack binPacingInstance;

		binPacingInstance.Init(
			LightmappingSetting::Instance()->lightmapSize,
			LightmappingSetting::Instance()->lightmapSize,
			False);

		//	scaling and packing.
		for (int32 i = 0; i < vertexCount; ++i)
		{
			vb[i].uv.x *= scale;
			vb[i].uv.y *= scale;

			rbp::Rect rect = binPacingInstance.Insert(
				ToUpper(vb[i].uv.x * LightmappingSetting::Instance()->lightmapSize),
				ToUpper(vb[i].uv.y * LightmappingSetting::Instance()->lightmapSize),
				rbp::MaxRectsBinPack::RectBestShortSideFit);

			vb[i].uv.x = 1.0f * (rect.x + rect.width) / LightmappingSetting::Instance()->lightmapSize;
			vb[i].uv.y = 1.0f * (rect.y + rect.height) / LightmappingSetting::Instance()->lightmapSize;
		}


		//result = UVAtlasPack(
		//	vb,
		//	ib,
		//	DXGI_FORMAT_R32_UINT,
		//	LightmappingSetting::Instance()->lightmapSize,
		//	LightmappingSetting::Instance()->lightmapSize,
		//	LightmappingSetting::Instance()->gutter,
		//	partitionResultAdjacency,
		//	Null,
		//	0);

		//result = UVAtlasCreate(
		//	pPosition3,
		//	vertexCount,
		//	pIndices,
		//	DXGI_FORMAT_R32_UINT,
		//	indexCount / 3,
		//	LightmappingSetting::Instance()->maxChartNumber,
		//	LightmappingSetting::Instance()->maxStrechSize,
		//	LightmappingSetting::Instance()->lightmapSize,
		//	LightmappingSetting::Instance()->lightmapSize,
		//	LightmappingSetting::Instance()->gutter,
		//	pAdjacency,
		//	Null,
		//	pIMTData,
		//	Null,
		//	0,
		//	UVATLAS_GEODESIC_FAST,//UVATLAS_DEFAULT,//UVATLAS_GEODESIC_FAST,
		//	vb,
		//	ib,
		//	&facePartitioning,
		//	&vertexRemapArray,
		//	&outStretch,
		//	&outCharts);

		if (pUV1s != Null)
			delete[] pUV1s;

		pUV1s = new Vector2[vertexCount];

		for (int32 i = 0; i < vertexCount; ++i)
		{
			pUV1s[i].x = vb[i].uv.x;
			pUV1s[i].y = vb[i].uv.y;
		}

		m_atlasWidth = LightmappingSetting::Instance()->lightmapSize;// *outStretch;
		m_atlasHeight = LightmappingSetting::Instance()->lightmapSize;// *outStretch;

		delete[] pPosition3;
		delete[] pIMTData;
		delete[] pAdjacency;
	}

	void StaticMesh::generateUV1Thekla()
	{
		Atlas_Input_Mesh input_mesh;

		input_mesh.vertex_count = vertexCount;
		input_mesh.vertex_array = new Atlas_Input_Vertex[vertexCount];

		for (int32 i = 0; i < vertexCount; ++i)
		{
			input_mesh.vertex_array[i].position[0] = pPositions[i].x;
			input_mesh.vertex_array[i].position[1] = pPositions[i].y;
			input_mesh.vertex_array[i].position[2] = pPositions[i].z;

			input_mesh.vertex_array[i].normal[0] = pNormals[i].x;
			input_mesh.vertex_array[i].normal[1] = pNormals[i].y;
			input_mesh.vertex_array[i].normal[2] = pNormals[i].z;

			input_mesh.vertex_array[i].uv[0] = pUV0s[i].x;
			input_mesh.vertex_array[i].uv[1] = pUV0s[i].y;

			if (pFirstColocals != Null)
				input_mesh.vertex_array[i].first_colocal = pFirstColocals[i];
			else
				input_mesh.vertex_array[i].first_colocal = i;
		}

		input_mesh.face_count = indexCount / 3;
		input_mesh.face_array = new Atlas_Input_Face[indexCount / 3];

		for (int32 i = 0; i < input_mesh.face_count; ++i)
		{
			input_mesh.face_array[i].vertex_index[0] = pIndices[i * 3 + 0];
			input_mesh.face_array[i].vertex_index[1] = pIndices[i * 3 + 1];
			input_mesh.face_array[i].vertex_index[2] = pIndices[i * 3 + 2];

			input_mesh.face_array[i].material_index = 0;
		}

		Atlas_Options atlas_options;

		atlas_options.charter = Atlas_Charter_Default;
		atlas_options.charter_options.witness.proxy_fit_metric_weight = 2.0f;
		atlas_options.charter_options.witness.roundness_metric_weight = 0.01f;
		atlas_options.charter_options.witness.straightness_metric_weight = 6.0f;
		atlas_options.charter_options.witness.normal_seam_metric_weight = 4.0f;
		atlas_options.charter_options.witness.texture_seam_metric_weight = 0.5f;
		atlas_options.charter_options.witness.max_chart_area = 1024.0f;
		atlas_options.charter_options.witness.max_boundary_length = FLT_MAX;

		atlas_options.mapper = Atlas_Mapper_Default;

		atlas_options.packer = Atlas_Packer_Default;
		atlas_options.packer_options.witness.packing_quality = 1;	//	1,atlas小;4,atlas大.
		atlas_options.packer_options.witness.texel_area = LightmappingSetting::Instance()->uvScale;
		atlas_options.packer_options.witness.block_align = true;
		atlas_options.packer_options.witness.conservative = false;

		Atlas_Error error = Atlas_Error_Success;
		Atlas_Output_Mesh * output_mesh = atlas_generate(&input_mesh, &atlas_options, &error);

		if (pUV1s != Null)
			delete[] pUV1s;

		pUV1s = new Vector2[vertexCount];
		
		for (int32 i = 0; i < vertexCount; ++i)
		{
			int32 index = output_mesh->vertex_array[i].xref;
		
			pUV1s[index].x = output_mesh->vertex_array[i].uv[0] / output_mesh->atlas_width;
			pUV1s[index].y = output_mesh->vertex_array[i].uv[1] / output_mesh->atlas_height;
		}

		m_atlasWidth = output_mesh->atlas_width;
		m_atlasHeight = output_mesh->atlas_height;

		delete[] input_mesh.vertex_array;
		delete[] input_mesh.face_array;

		atlas_free(output_mesh);
	}
	
	void StaticMesh::scaleUV1()
	{
		float positionAreaTotal = 0;
		float uvAreaTotal = 0;

		for (int32 triangleIndex = 0; triangleIndex < indexCount / 3; ++triangleIndex)
		{
			Vector3 pos0 = pPositions[triangleIndex * 3 + 0];
			Vector3 pos1 = pPositions[triangleIndex * 3 + 1];
			Vector3 pos2 = pPositions[triangleIndex * 3 + 2];

			float positionArea = getTriangleArea(pos0, pos1, pos2);

			positionAreaTotal += positionArea;

			Vector2 uv0 = pUV1s[triangleIndex * 3 + 0];
			Vector2 uv1 = pUV1s[triangleIndex * 3 + 1];
			Vector2 uv2 = pUV1s[triangleIndex * 3 + 2];

			float uvArea = getTriangleArea(uv0, uv1, uv2);

			uvAreaTotal += uvArea;
		}

		float division = uvAreaTotal / positionAreaTotal;
		float scale = LightmappingSetting::Instance()->uvScale / division;

		float maxU = 0;
		float maxV = 0;

		for (int32 i = 0; i < vertexCount; ++i)
		{
			pUV1s[i].x *= scale;
			pUV1s[i].y *= scale;

			if (pUV1s[i].x > maxU)
				maxU = pUV1s[i].x;

			if (pUV1s[i].y > maxV)
				maxV = pUV1s[i].y;
		}

		m_atlasWidth = static_cast<int32>(maxU * LightmappingSetting::Instance()->lightmapSize);
		m_atlasHeight = static_cast<int32>(maxV * LightmappingSetting::Instance()->lightmapSize);
	}

	ErrorCode StaticMesh::uploadToGPU()
	{
		if (!pIndices ||
			indexCount == 0 ||
			vertexCount == 0)
		{
			return ErrorCode_InvalidData;
		}

		int32 vertexRawDataSize = 0;
		int32 vertexRawDataStride = 0;

		int32 positionSize = pPositions ? sizeof(pPositions[0]) * vertexCount : 0;
		vertexRawDataSize += positionSize;
		vertexRawDataStride += positionSize / vertexCount;

		int32 normalSize = pNormals ? sizeof(pNormals[0]) * vertexCount : 0;
		vertexRawDataSize += normalSize;
		vertexRawDataStride += normalSize / vertexCount;

		int32 tangentSize = pTangents ? sizeof(pTangents[0]) * vertexCount : 0;
		vertexRawDataSize += tangentSize;
		vertexRawDataStride += tangentSize / vertexCount;

		int32 binormalSize = pBinormals ? sizeof(pBinormals[0]) * vertexCount : 0;
		vertexRawDataSize += binormalSize;
		vertexRawDataStride += binormalSize / vertexCount;

		int32 colorSize = pColors ? sizeof(pColors[0]) * vertexCount : 0;
		vertexRawDataSize += colorSize;
		vertexRawDataStride += colorSize / vertexCount;

		int32 uv0Size = pUV0s ? sizeof(pUV0s[0]) * vertexCount : 0;
		vertexRawDataSize += uv0Size;
		vertexRawDataStride += uv0Size / vertexCount;

		int32 uv1Size = pUV1s ? sizeof(pUV1s[0]) * vertexCount : 0;
		vertexRawDataSize += uv1Size;
		vertexRawDataStride += uv1Size / vertexCount;

		int32 uv2Size = pUV2s ? sizeof(pUV2s[0]) * vertexCount : 0;
		vertexRawDataSize += uv2Size;
		vertexRawDataStride += uv2Size / vertexCount;

		int32 uv3Size = pUV3s ? sizeof(pUV3s[0]) * vertexCount : 0;
		vertexRawDataSize += uv3Size;
		vertexRawDataStride += uv3Size / vertexCount;

		int32 uv4Size = pUV4s ? sizeof(pUV4s[0]) * vertexCount : 0;
		vertexRawDataSize += uv4Size;
		vertexRawDataStride += uv4Size / vertexCount;

		int32 uv5Size = pUV5s ? sizeof(pUV5s[0]) * vertexCount : 0;
		vertexRawDataSize += uv5Size;
		vertexRawDataStride += uv5Size / vertexCount;

		int32 uv6Size = pUV6s ? sizeof(pUV6s[0]) * vertexCount : 0;
		vertexRawDataSize += uv6Size;
		vertexRawDataStride += uv6Size / vertexCount;

		int32 uv7Size = pUV7s ? sizeof(pUV7s[0]) * vertexCount : 0;
		vertexRawDataSize += uv7Size;
		vertexRawDataStride += uv7Size / vertexCount;

		m_vertexRawDataSize = vertexRawDataSize;
		m_vertexRawDataStride = vertexRawDataStride;

		if (positionSize != m_positionSize ||
			normalSize != m_normalSize ||
			tangentSize != m_tangentSize ||
			binormalSize != m_binormalSize ||
			colorSize != m_colorSize ||
			uv0Size != m_uv0Size ||
			uv1Size != m_uv1Size ||
			uv2Size != m_uv2Size ||
			uv3Size != m_uv3Size ||
			uv4Size != m_uv4Size ||
			uv5Size != m_uv5Size ||
			uv6Size != m_uv6Size ||
			uv7Size != m_uv7Size)
		{
			if (m_pVertexRawData)
				delete[] m_pVertexRawData;

			m_pVertexRawData = new float[vertexRawDataSize];

			m_pVertexBuffer->Recreate(BufferTarget_ArrayBuffer, BufferUsage_StaticDraw, vertexRawDataSize);
		}

		m_positionSize = positionSize;
		m_normalSize = normalSize;
		m_tangentSize = tangentSize;
		m_binormalSize = binormalSize;
		m_colorSize = colorSize;
		m_uv0Size = uv0Size;
		m_uv1Size = uv1Size;
		m_uv2Size = uv2Size;
		m_uv3Size = uv3Size;
		m_uv4Size = uv4Size;
		m_uv5Size = uv5Size;
		m_uv6Size = uv6Size;
		m_uv7Size = uv7Size;

		int32 offset = 0;

		for (int32 i = 0; i < vertexCount; ++i)
		{
			if (positionSize > 0)
			{
				memcpy(m_pVertexRawData + offset, pPositions + i, sizeof(pPositions[0]));
				offset += GetComponentCount(pPositions[0]);
			}

			if (normalSize > 0)
			{
				memcpy(m_pVertexRawData + offset, pNormals + i, sizeof(pNormals[0]));
				offset += GetComponentCount(pNormals[0]);
			}

			if (tangentSize > 0)
			{
				memcpy(m_pVertexRawData + offset, pTangents + i, sizeof(pTangents[0]));
				offset += GetComponentCount(pTangents[0]);
			}

			if (binormalSize > 0)
			{
				memcpy(m_pVertexRawData + offset, pBinormals + i, sizeof(pBinormals[0]));
				offset += GetComponentCount(pBinormals[0]);
			}

			if (colorSize > 0)
			{
				memcpy(m_pVertexRawData + offset, pColors + i, sizeof(pColors[0]));
				offset += GetComponentCount(pColors[0]);
			}

			if (uv0Size > 0)
			{
				memcpy(m_pVertexRawData + offset, pUV0s + i, sizeof(pUV0s[0]));
				offset += GetComponentCount(pUV0s[0]);
			}

			if (uv1Size > 0)
			{
				memcpy(m_pVertexRawData + offset, pUV1s + i, sizeof(pUV1s[0]));
				offset += GetComponentCount(pUV1s[0]);
			}

			if (uv2Size > 0)
			{
				memcpy(m_pVertexRawData + offset, pUV2s + i, sizeof(pUV2s[0]));
				offset += GetComponentCount(pUV2s[0]);
			}

			if (uv3Size > 0)
			{
				memcpy(m_pVertexRawData + offset, pUV3s + i, sizeof(pUV3s[0]));
				offset += GetComponentCount(pUV3s[0]);
			}

			if (uv4Size > 0)
			{
				memcpy(m_pVertexRawData + offset, pUV4s + i, sizeof(pUV4s[0]));
				offset += GetComponentCount(pUV4s[0]);
			}

			if (uv5Size > 0)
			{
				memcpy(m_pVertexRawData + offset, pUV5s + i, sizeof(pUV5s[0]));
				offset += GetComponentCount(pUV5s[0]);
			}

			if (uv6Size > 0)
			{
				memcpy(m_pVertexRawData + offset, pUV6s + i, sizeof(pUV6s[0]));
				offset += GetComponentCount(pUV6s[0]);
			}

			if (uv7Size > 0)
			{
				memcpy(m_pVertexRawData + offset, pUV7s + i, sizeof(pUV7s[0]));
				offset += GetComponentCount(pUV7s[0]);
			}
		}

		m_pVertexBufferLayout->Activate();
		m_pVertexBuffer->Activate(BufferTarget_ArrayBuffer);
		m_pVertexBuffer->UploadData(BufferTarget_ArrayBuffer, 0, vertexRawDataSize, m_pVertexRawData);

		int32 pointerOffset = 0;

		if (positionSize > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				positionSlotIndex,
				GetComponentCount(pPositions[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));	//	这有一个警告

			pointerOffset += sizeof(pPositions[0]);
		}

		if (normalSize > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				normalSlotIndex,
				GetComponentCount(pNormals[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pNormals[0]);
		}

		if (tangentSize > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				tangentSlotIndex,
				GetComponentCount(pTangents[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pTangents[0]);
		}

		if (binormalSize > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				binormalSlotIndex,
				GetComponentCount(pBinormals[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pBinormals[0]);
		}

		if (colorSize > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				colorSlotIndex,
				GetComponentCount(pColors[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				(void *)pointerOffset);

			pointerOffset += sizeof(pColors[0]);
		}

		if (uv0Size > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				uv0SlotIndex,
				GetComponentCount(pUV0s[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pUV0s[0]);
		}

		if (uv1Size > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				uv1SlotIndex,
				GetComponentCount(pUV1s[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pUV1s[0]);
		}

		if (uv2Size > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				uv2SlotIndex,
				GetComponentCount(pUV2s[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pUV2s[0]);
		}

		if (uv3Size > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				uv3SlotIndex,
				GetComponentCount(pUV3s[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pUV3s[0]);
		}

		if (uv4Size > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				uv4SlotIndex,
				GetComponentCount(pUV4s[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pUV4s[0]);
		}

		if (uv5Size > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				uv5SlotIndex,
				GetComponentCount(pUV5s[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pUV5s[0]);
		}

		if (uv6Size > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				uv6SlotIndex,
				GetComponentCount(pUV6s[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pUV6s[0]);
		}

		if (uv7Size > 0)
		{
			m_pVertexBufferLayout->SetSlotElement(
				uv7SlotIndex,
				GetComponentCount(pUV7s[0]),
				GLDataType_Float,
				False,
				vertexRawDataStride,
				reinterpret_cast<void *>(pointerOffset));

			pointerOffset += sizeof(pUV7s[0]);
		}

		if (indexCount != m_indexCount)
		{
			m_pIndexBuffer->Recreate(BufferTarget_ElementArray, BufferUsage_StaticDraw, sizeof(m_pVertexRawData[0]) * indexCount);
		}

		m_indexCount = indexCount;

		m_pIndexBuffer->Activate(BufferTarget_ElementArray);
		m_pIndexBuffer->UploadData(BufferTarget_ElementArray, 0, sizeof(m_pVertexRawData[0]) * indexCount, pIndices);

		m_pVertexBufferLayout->Inactivate();

		return ErrorCode_OK;
	}

	void StaticMesh::load()
	{
		if (generateLightmapUV)
		{
			generateUV1Thekla();
		}

		uploadToGPU();
	}

	StaticMesh::StaticMesh()
		:m_pVertexRawData(Null),
		m_local2World(Matrix4x4Identify),
		m_pVertexBuffer(std::make_unique<GLBuffer>()),
		m_pIndexBuffer(std::make_unique<GLBuffer>()),
		m_pVertexBufferLayout(std::make_unique<GLVertexBufferLayout>()),
		vertexCount(0),
		m_positionSize(0),
		m_normalSize(0),
		m_tangentSize(0),
		m_binormalSize(0),
		m_colorSize(0),
		m_uv0Size(0),
		m_uv1Size(0),
		m_uv2Size(0),
		m_uv3Size(0),
		m_uv4Size(0),
		m_uv5Size(0),
		m_uv6Size(0),
		m_uv7Size(0),
		m_indexCount(0),
		m_lightmapUVRotated(False),
		pPositions(Null),
		pNormals(Null),
		pTangents(Null),
		pBinormals(Null),
		pColors(Null),
		pUV0s(Null),
		pUV1s(Null),
		pUV2s(Null),
		pUV3s(Null),
		pUV4s(Null),
		pUV5s(Null),
		pUV6s(Null),
		pUV7s(Null),
		pIndices(Null),
		generateLightmapUV(True),
		pFirstColocals(Null)
	{
	}

	void StaticMesh::BeginUse()
	{
		if (!m_beingUsed)
		{
			load();
			m_beingUsed = True;
		}
	}

	void StaticMesh::Reload()
	{
		load();
	}

	void StaticMesh::BeforeSave()
	{
	}

	void StaticMesh::AfterLoad()
	{
	}

	void StaticMesh::Activate() const
	{
		m_pVertexBufferLayout->Activate();
	}

	void StaticMesh::Inactivate() const
	{
		m_pVertexBufferLayout->Inactivate();
	}

	float * StaticMesh::GetVertexRawData() const
	{
		return m_pVertexRawData;
	}

	Core::int32 StaticMesh::GetVertexRawDataSize() const
	{
		return m_vertexRawDataSize;
	}

	Core::int32 StaticMesh::GetVertexRawDataStride() const
	{
		return m_vertexRawDataStride;
	}

	int32 StaticMesh::GetPositionSize() const
	{
		return m_positionSize;
	}

	int32 StaticMesh::GetNormalSize() const
	{
		return m_normalSize;
	}

	int32 StaticMesh::GetTangentSize() const
	{
		return m_tangentSize;
	}

	int32 StaticMesh::GetBinormalSize() const
	{
		return m_binormalSize;
	}

	int32 StaticMesh::GetColorSize() const
	{
		return m_colorSize;
	}

	int32 StaticMesh::GetUV0Size() const
	{
		return m_uv0Size;
	}

	int32 StaticMesh::GetUV1Size() const
	{
		return m_uv1Size;
	}

	int32 StaticMesh::GetUV2Size() const
	{
		return m_uv2Size;
	}

	int32 StaticMesh::GetUV3Size() const
	{
		return m_uv3Size;
	}

	int32 StaticMesh::GetUV4Size() const
	{
		return m_uv4Size;
	}

	int32 StaticMesh::GetUV5Size() const
	{
		return m_uv5Size;
	}

	int32 StaticMesh::GetUV6Size() const
	{
		return m_uv6Size;
	}

	int32 StaticMesh::GetUV7Size() const
	{
		return m_uv7Size;
	}

	Core::int32 StaticMesh::GetLightmapAtlasWidth() const
	{
		return m_atlasWidth;
	}

	Core::int32 StaticMesh::GetLightmapAtlasHeight() const
	{
		return m_atlasHeight;
	}

	void StaticMesh::RotateUV1HalfPi()
	{
		if (!m_lightmapUVRotated)
		{
			for (int32 i = 0; i < vertexCount; ++i)
			{
				float temp = pUV1s[i].x;

				pUV1s[i].x = pUV1s[i].y;
				pUV1s[i].y = temp;
			}

			//	XXX:	回头可以看看能不能单独更新buffer一部分.
			uploadToGPU();

			m_lightmapUVRotated = True;
		}
	}

	void StaticMesh::SetControlPointCount(int32 controlPointCount)
	{
		m_controlPointCount = controlPointCount;
	}

	void StaticMesh::SetLocal2World(Matrix4x4 local2World)
	{
		m_local2World = local2World;
	}

	Core::Matrix4x4 StaticMesh::GetLocal2World() const
	{
		return m_local2World;
	}

	StaticMesh::~StaticMesh()
	{
		if (pPositions)
			delete[] pPositions;

		if (pNormals)
			delete[] pNormals;

		if (pTangents)
			delete[] pTangents;

		if (pBinormals)
			delete[] pBinormals;

		if (pColors)
			delete[] pColors;

		if (pUV0s)
			delete[] pUV0s;

		if (pUV1s)
			delete[] pUV1s;

		if (pUV2s)
			delete[] pUV2s;

		if (pUV3s)
			delete[] pUV3s;

		if (pUV4s)
			delete[] pUV4s;

		if (pUV5s)
			delete[] pUV5s;

		if (pUV6s)
			delete[] pUV6s;

		if (pUV7s)
			delete[] pUV7s;

		if (pIndices)
			delete[] pIndices;

		if (pFirstColocals)
			delete[] pFirstColocals;
	}
}