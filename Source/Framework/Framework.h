#pragma once
#include <BaseInclude.h>
#include "../Asset/World.h"
#include "../System/RenderSystem.h"
#include "../RHI/VulkanRHI/VulkanDevice.h"
#include "../System/CameraSystem.h"
#include "../Component/InputStateData.h"

namespace Core
{
	class Framework
	{
	private:
		std::shared_ptr<World> m_currentWorld;

		std::unique_ptr<RenderSystem> m_renderSystem;
		std::unique_ptr<CameraSystem> m_cameraSystem;

	public:
		Framework();
		void SetCurrentWorld(std::shared_ptr<World> currentWorld);
		void Initialize(VulkanDevice * pDevice, float ascept);
		void Tick(float deltaTime, VulkanDevice * pDevice, InputStateData * pInputStateData);
		~Framework();
	};
}