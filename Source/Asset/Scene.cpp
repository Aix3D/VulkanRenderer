#include "Scene.h"
#include "..\Helper\Helper.h"
#include <algorithm>

using namespace std;
using namespace ctd;

namespace Core
{
	Scene::Scene()
		:m_camera(std::make_unique<Camera>())
	{
	}

	void Scene::BeginUse()
	{
	}
	
	void Scene::Reload()
	{
	}
	
	void Scene::BeforeSave()
	{
		for (vector<std::shared_ptr<Object>>::iterator iter = serializedObjects.begin();
			iter != serializedObjects.end();
			++iter)
		{
			iter->get()->BeforeSave();
		}
	}
	
	void Scene::AfterLoad()
	{
		for (vector<std::shared_ptr<Object>>::iterator iter = serializedObjects.begin();
			iter != serializedObjects.end();
			++iter)
		{
			iter->get()->AfterLoad();
		}
	}

	void Scene::Initialize(OpenGLDevice * pDevice, RLDevice * pRLDevice, int32 width, int32 height)
	{
		m_camera->Initialize(pDevice, width, height);

		m_rlUniformBuffer = std::make_shared<RLBuffer>(RLBufferTarget_UniformBlockBuffer);
		m_rlUniformBuffer->name = "LightingSettings";
		m_rlUniformBuffer->AllocateMemorySpace(sizeof(LightingSettings), RLBufferUsage_DynamicDraw);
	}

	void Scene::Tick(float deltaTime, OpenGLDevice * pDevice, RLDevice * pRLDevice, const InputState & inputState)
	{
		for (ctd::vector<std::shared_ptr<Object>> ::iterator iter = objects.begin();
			iter != objects.end();
			++iter)
		{
			iter->get()->Tick(deltaTime, pDevice, pRLDevice);
		}

		m_camera->Tick(deltaTime, inputState);
	}
	
	void Scene::Render(OpenGLDevice * pDevice, int32 width, int32 height)
	{
 		pDevice->BeforeRender(width, height);
		m_camera->UpdataGLParam(pDevice);

		Vector3 lightDir = Normalize(m_dirLightRef.lock()->GetYDirection());
		pDevice->UploadGlobalShaderData(GLShaderDataAlias_LightParam, sizeof(lightDir), &lightDir);

		for (ctd::vector<std::shared_ptr<Object>> ::iterator iter = objects.begin();
			iter != objects.end();
			++iter)
		{
			iter->get()->Render(pDevice);
		}

		pDevice->AfterRender();
	}

	void Scene::BeginBaking(OpenGLDevice * pGLDevice, RLDevice * pRLDevice, int32 width, int32 height)
	{
		m_objectCursor = 0;
		pGLDevice->BeforeRender(width, height, 0);

		m_camera->UpdataGLParam(pGLDevice);
		UpdateRLBeforeBaking(pRLDevice);
	}

	Core::Bool Scene::BakingFinished() const
	{
		return m_objectCursor == objects.size();
	}

	Core::Object * Scene::PoolBakingObject() const
	{
		return objects[m_objectCursor].get();
	}

	void Scene::SkipOneObject()
	{
		++m_objectCursor;
	}

	Bool Scene::BakeOneObject(OpenGLDevice * pDevice)
	{
		objects[m_objectCursor]->Bake(pDevice);

		++m_objectCursor;

		return True;
	}

	void Scene::EndBaking(OpenGLDevice * pDevice)
	{
		pDevice->AfterRender();
	}

	void Scene::UpdateRLParameter(RLDevice * pDevice, Vector4 jitter)
	{
		m_camera->UpdataRLParam(pDevice);

		for (ctd::vector<std::shared_ptr<Object>> ::iterator iter = objects.begin();
			iter != objects.end();
			++iter)
		{
			iter->get()->rlRenderableUnit->Activate();
			pDevice->SetTransformMatrix(iter->get()->GetObject2WorldMatrix());
			//	FIXME:	这里3个贴图不应该每一帧都设置.
			iter->get()->rlRenderableUnit->SetAlbdeoTexture();
			iter->get()->rlRenderableUnit->SetAlbdeoNormalMap();
			iter->get()->rlRenderableUnit->SetAlbedoColor();
			iter->get()->rlRenderableUnit->Inactivate();
		}

		if (!m_dirLightRef.expired())
		{
			m_rlUniformBuffer->Activate();
			m_uniformBufferData = (LightingSettings *)m_rlUniformBuffer->Map(RLBufferAccessFlag_ReadWrite);
			Vector4 lightDir4 = *(m_dirLightRef.lock()->GetObject2WorldMatrix()) * Up4;
			Vector3 lightDir = Vector3(lightDir4.x, lightDir4.y, lightDir4.z);
			m_uniformBufferData->lightDir[0] = lightDir.x;
			m_uniformBufferData->lightDir[1] = lightDir.y;
			m_uniformBufferData->lightDir[2] = lightDir.z;
			m_uniformBufferData->lightDir[3] = 1.0f;
			m_uniformBufferData->jitter[0] = jitter.x;
			m_uniformBufferData->jitter[1] = jitter.y;
			m_uniformBufferData->jitter[2] = jitter.z;
			m_uniformBufferData->jitter[3] = jitter.w;
			m_uniformBufferData->lightPrimitive = m_dirLightRef.lock()->rlRenderableUnit->GetRawPrimitive();
			m_rlUniformBuffer->Unmap();
			m_rlUniformBuffer->Inactivate();
		}
	}

	void Scene::UpdateRLBeforeBaking(RLDevice * pDevice)
	{
		for (ctd::vector<std::shared_ptr<Object>> ::iterator iter = objects.begin();
			iter != objects.end();
			++iter)
		{
			iter->get()->rlRenderableUnit->Activate();
			pDevice->SetTransformMatrix(iter->get()->GetObject2WorldMatrix());
			iter->get()->rlRenderableUnit->SetAlbdeoTexture();
			iter->get()->rlRenderableUnit->SetAlbdeoNormalMap();
			iter->get()->rlRenderableUnit->SetAlbedoColor();
			iter->get()->rlRenderableUnit->Inactivate();
		}
	}

	void Scene::UpdateRLPerStep(RLDevice * pDevice, Vector4 jitter)
	{
		if (!m_dirLightRef.expired())
		{
			m_rlUniformBuffer->Activate();
			m_uniformBufferData = (LightingSettings *)m_rlUniformBuffer->Map(RLBufferAccessFlag_ReadWrite);
			Vector4 lightDir4 = *(m_dirLightRef.lock()->GetObject2WorldMatrix()) * Up4;
			Vector3 lightDir = Vector3(lightDir4.x, lightDir4.y, lightDir4.z);
			m_uniformBufferData->lightDir[0] = lightDir.x;
			m_uniformBufferData->lightDir[1] = lightDir.y;
			m_uniformBufferData->lightDir[2] = lightDir.z;
			m_uniformBufferData->lightDir[3] = 1.0f;
			m_uniformBufferData->jitter[0] = jitter.x;
			m_uniformBufferData->jitter[1] = jitter.y;
			m_uniformBufferData->jitter[2] = jitter.z;
			m_uniformBufferData->jitter[3] = jitter.w;
			m_uniformBufferData->lightPrimitive = m_dirLightRef.lock()->rlRenderableUnit->GetRawPrimitive();
			m_rlUniformBuffer->Unmap();
			m_rlUniformBuffer->Inactivate();
		}
	}

	Core::Camera * Scene::GetCamera()
	{
		return m_camera.get();
	}

	std::shared_ptr<Core::RLBuffer> Scene::GetRLUniformBuffer() const
	{
		return m_rlUniformBuffer;
	}

	void Scene::AddObject(std::shared_ptr<Object> object, Bool needSerialization)
	{
		objects.push_back(object);

		if (needSerialization)
			serializedObjects.push_back(object);
	}

	void Scene::AddLight(std::shared_ptr<Object> object, Bool needSerialization)
	{
		objects.push_back(object);

		m_dirLightRef = object; 

		if (needSerialization)
			serializedObjects.push_back(object);
	}

	Scene::~Scene()
	{

	}
}