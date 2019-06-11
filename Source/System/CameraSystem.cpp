#include "CameraSystem.h"
#include <MathGlobal.h>

namespace Core
{
	CameraSystem::CameraSystem()
	{

	}

	void CameraSystem::Initialize(VulkanDevice * pDevice, float ascept, GlobalRenderData * pGlobalRenderData, TransformData * pTransformData, CameraData * pCameraData)
	{
		pTransformData->position = Vector3(0, 2.0f, 10.0f);
		pTransformData->rotation = Vector3Dummy;
		pTransformData->scale = Vector3(1.0, 1.0, 1.0);

		pCameraData->lookAtCenter = Vector3(0, 2.0f, 0);
		pCameraData->lookAtDir = Vector3(0, 0, -1.0f);
		pCameraData->cameraUp = Up;
		pCameraData->fovY = 90.0f * Deg2Rad;
		pCameraData->zNear = 0.1f;
		pCameraData->zFar = 10000.0f;

		pCameraData->m_viewMatrix = LookAt(pTransformData->position, pTransformData->position + pCameraData->lookAtDir, pCameraData->cameraUp);
		pCameraData->m_perspectiveMatrix = Perspective(pCameraData->fovY, ascept, pCameraData->zNear, pCameraData->zFar);

		pGlobalRenderData->uniformBuffer = pDevice->CreateBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			sizeof(pGlobalRenderData->rawUniformBuffer));

		pGlobalRenderData->uniformBuffer->SetDescriptorBufferInfo();

		pGlobalRenderData->rawUniformBuffer.model = pCameraData->m_viewMatrix;
		pGlobalRenderData->rawUniformBuffer.projection = pCameraData->m_perspectiveMatrix;

		if (!pGlobalRenderData->uniformBuffer->mapped)
			pGlobalRenderData->uniformBuffer->Map(pDevice->GetLogicalDevice(), 0, sizeof(pGlobalRenderData->rawUniformBuffer));

		memcpy(pGlobalRenderData->uniformBuffer->mapped, &pGlobalRenderData->rawUniformBuffer, sizeof(pGlobalRenderData->rawUniformBuffer));

		pGlobalRenderData->uniformBuffer->Unmap(pDevice->GetLogicalDevice(), sizeof(pGlobalRenderData->rawUniformBuffer));
	}

	void CameraSystem::Tick(float deltaTime, VulkanDevice * pDevice, GlobalRenderData * pGlobalRenderData, const InputStateData * pInputState, TransformData * pTransformData, CameraData * pCameraData)
	{
		if (pInputState->inlvaid)
			return;

		float moveSpeed = deltaTime * pInputState->zPos;
		float rotateSpeed = 3.0f * deltaTime;

		if (pInputState->leftShiftDown)
		{
			moveSpeed *= 2.0f;
			rotateSpeed *= 2.0f;
		}

		if (pInputState->wDown)
			pTransformData->position += GetZAxis(pCameraData->m_viewMatrix) * -moveSpeed;

		if (pInputState->sDown)
			pTransformData->position += GetZAxis(pCameraData->m_viewMatrix) * moveSpeed;

		if (pInputState->aDown)
			pTransformData->position += GetXAxis(pCameraData->m_viewMatrix) * -moveSpeed;

		if (pInputState->dDown)
			pTransformData->position += GetXAxis(pCameraData->m_viewMatrix) * moveSpeed;

		if (pInputState->qDown)
			pTransformData->position += GetYAxis(pCameraData->m_viewMatrix) * -moveSpeed;

		if (pInputState->eDown)
			pTransformData->position += GetYAxis(pCameraData->m_viewMatrix) * moveSpeed;

		//	Camera rotate around pivot
		//if (inputState.leftBuffonDown && inputState.mouseIsMoveing)
		//{
		//	int32 screenXDelta = inputState.xPos - inputState.xPosPrev;
		//	int32 screenYDelta = inputState.yPos - inputState.yPosPrev;
		//
		//	if (Abs(screenXDelta) > Abs(screenYDelta))
		//	{
		//		Vector3 center2Camera = position - lookAtCenter;
		//		Vector3 axis = Up;
		//		float angleDelta = screenXDelta * Deg2Rad * 1.5f * deltaTime;
		//		Matrix3x3 rotationMatrix = Rotate(Matrix3x3Identify, -angleDelta, axis);
		//		center2Camera = rotationMatrix * center2Camera;
		//		cameraUp = rotationMatrix * cameraUp;
		//	
		//		position = lookAtCenter + center2Camera;
		//	}
		//	else if (Abs(screenYDelta) > Abs(screenXDelta))
		//	{
		//		Vector3 center2Camera = position - lookAtCenter;
		//		Vector3 axis = GetXAxis(m_viewMatrix);
		//		float angleDelta = screenYDelta * Deg2Rad * 1.5f * deltaTime;
		//
		//		const float maxRestriction = PI;
		//		const float minRestriction = 0;
		//
		//		if (m_yAccumulateAngle + angleDelta > maxRestriction)
		//			angleDelta = maxRestriction - m_yAccumulateAngle;
		//		else if (m_yAccumulateAngle + angleDelta < minRestriction)
		//			angleDelta = minRestriction - m_yAccumulateAngle;
		//
		//		m_yAccumulateAngle += angleDelta;
		//
		//		Matrix3x3 rotationMatrix = Rotate(Matrix3x3Identify, -angleDelta, axis);
		//		center2Camera = rotationMatrix * center2Camera;
		//		cameraUp = rotationMatrix * cameraUp;
		//
		//		position = lookAtCenter + center2Camera;
		//	}
		//}
		//
		//if (inputState.zDelta != 0)
		//{
		//	Vector3 forward = GetZAxis(m_viewMatrix);
		//
		//	float zDelta = inputState.zDelta * -0.1f * deltaTime;
		//
		//	position += forward * zDelta;
		//}

		//////////////////////////////////////////////////////////////////////////

		//	Rotate around camera itself
		if (pInputState->rightBuffonDown && pInputState->mouseIsMoveing)
		{
			int32 screenXDelta = pInputState->xPos - pInputState->xPosPrev;
			int32 screenYDelta = pInputState->yPos - pInputState->yPosPrev;

			pCameraData->m_viewMatrix = Rotate(pCameraData->m_viewMatrix, screenXDelta * rotateSpeed * Deg2Rad, GetYAxis(pCameraData->m_viewMatrix));
			pCameraData->m_viewMatrix = Rotate(pCameraData->m_viewMatrix, screenYDelta * rotateSpeed * Deg2Rad, GetXAxis(pCameraData->m_viewMatrix));

			pCameraData->lookAtDir = -GetZAxis(pCameraData->m_viewMatrix);
		}

		pCameraData->m_viewMatrix = LookAt(pTransformData->position, pTransformData->position + pCameraData->lookAtDir, pCameraData->cameraUp);
		pCameraData->m_viewMatrixInv = Inverse(pCameraData->m_viewMatrix);
		pCameraData->m_viewProjectionMatrix = pCameraData->m_perspectiveMatrix * pCameraData->m_viewMatrix;
	
		pGlobalRenderData->rawUniformBuffer.model = pCameraData->m_viewMatrix;
		pGlobalRenderData->rawUniformBuffer.projection = pCameraData->m_perspectiveMatrix;

		if (!pGlobalRenderData->uniformBuffer->mapped)
			pGlobalRenderData->uniformBuffer->Map(pDevice->GetLogicalDevice(), 0, sizeof(pGlobalRenderData->rawUniformBuffer));

		memcpy(pGlobalRenderData->uniformBuffer->mapped, &pGlobalRenderData->rawUniformBuffer, sizeof(pGlobalRenderData->rawUniformBuffer));

		//m_pUniformBuffer.lock()->Unmap(m_logicalDevice, sizeof(uboVS));
	}

	CameraSystem::~CameraSystem()
	{

	}
}