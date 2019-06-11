#include "Framework.h"

namespace Core
{
	Framework::Framework()
	{

	}

	void Framework::SetCurrentWorld(std::shared_ptr<World> currentWorld)
	{
		m_currentWorld = currentWorld;
	}

	void Framework::Initialize(VulkanDevice * pDevice, float ascept)
	{
		m_cameraSystem->Initialize(pDevice, ascept, GlobalRenderData::Instance(), m_currentWorld->camera->transformData.get(), m_currentWorld->camera->cameraData.get());

		for (std::map<int, std::unique_ptr<Entity>>::iterator iter = m_currentWorld->entityDict.begin(); iter != m_currentWorld->entityDict.end(); ++iter)
		{
			if (iter->second->renderData.get() != Null)
			{
				m_renderSystem->Initialize(pDevice, GlobalRenderData::Instance(), iter->second->renderData.get());
			}
		}
	}

	void Framework::Tick(float deltaTime, VulkanDevice * pDevice, InputStateData * pInputStateData)
	{
		m_cameraSystem->Tick(deltaTime, pDevice, GlobalRenderData::Instance(), pInputStateData, m_currentWorld->camera.get()->transformData.get(), m_currentWorld->camera.get()->cameraData.get());
	
		for (std::map<int, std::unique_ptr<Entity>>::iterator iter = m_currentWorld->entityDict.begin(); iter != m_currentWorld->entityDict.end(); ++iter)
		{
			if (iter->second->renderData.get() != Null)
			{
				m_renderSystem->Tick(deltaTime, iter->second->renderData.get());
			}
		}
	}

	Framework::~Framework()
	{

	}
}