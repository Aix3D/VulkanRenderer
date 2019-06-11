#pragma once
#include "ISystem.h"
#include "../Component/RenderableEntityData.h"
#include "../Asset/StaticMesh.h"
#include "../Asset/Material.h"
#include "../Component/GlobalRenderData.h"
#include "../RHI/VulkanRHI/VulkanDevice.h"

namespace Core
{
	class RenderSystem : public ISystem
	{
	public:
		RenderSystem();
		void Initialize(VulkanDevice * pDevice, GlobalRenderData * pGlobalRenderData, RenderableEntityData * pRenderableEntityData);
		void Tick(float deltaTime, const RenderableEntityData * pData);
		virtual ~RenderSystem();
	};
}