#pragma once
#include "ISystem.h"
#include "../Component/TransformData.h"
#include "../Component/CameraData.h"
#include "../Component/InputStateData.h"
#include "../Component/GlobalRenderData.h"
#include "../RHI/VulkanRHI/VulkanDevice.h"

namespace Core
{
	class CameraSystem : public ISystem
	{
	public:
		CameraSystem();
		void Initialize(VulkanDevice * pDevice, float ascept, GlobalRenderData * pGlobalRenderData, TransformData * pTransformData, CameraData * pCameraData);
		void Tick(float deltaTime, VulkanDevice * pDevice, GlobalRenderData * pGlobalRenderData, const InputStateData * pInputState, TransformData * pTransformData, CameraData * pCameraData);
		virtual ~CameraSystem();
	};
}