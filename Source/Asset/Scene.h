#pragma once
#include <BaseInclude.h>
#include "..\Object\Object.h"
#include "..\Object\Camera.h"
#include "..\RHI\OpenRLRHI\RLDevice.h"
#include "Prefab.h"
#include "../3rdParty/cereal/types/vector.hpp"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class Scene : public IAsset
	{
	private:
		struct LightingSettings
		{
			RLvec4      lightDir;
			RLvec4		jitter;
			RLprimitive lightPrimitive;
		};

	private:
		friend class cereal::access;

		std::unique_ptr<Camera> m_camera;

		std::weak_ptr<Object> m_dirLightRef;
		std::shared_ptr<RLBuffer> m_rlUniformBuffer;

		LightingSettings * m_uniformBufferData;

		int32 m_objectCursor;

	public:
		ctd::vector<std::shared_ptr<Object>> objects;
		ctd::vector<std::shared_ptr<Object>> serializedObjects;

		Scene();
		Scene(Scene &) {}
		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual void BeforeSave() override;
		virtual void AfterLoad() override;
		void Initialize(OpenGLDevice * pDevice, RLDevice * pRLDevice, int32 width, int32 height);
		void Tick(float deltaTime, OpenGLDevice * pDevice, RLDevice * pRLDevice, const InputState & inputState);
		void Render(OpenGLDevice * pDevice, int32 width, int32 height);
		void BeginBaking(OpenGLDevice * pGLDevice, RLDevice * pRLDevice, int32 width, int32 height);
		Bool BakingFinished()const;
		Object * PoolBakingObject() const;
		void SkipOneObject();
		Bool BakeOneObject(OpenGLDevice * pDevice);
		void EndBaking(OpenGLDevice * pDevice);
		void UpdateRLParameter(RLDevice * pDevice, Vector4 jitter);
		void UpdateRLBeforeBaking(RLDevice * pDevice);
		void UpdateRLPerStep(RLDevice * pDevice, Vector4 jitter);
		Camera * GetCamera();
		std::shared_ptr<RLBuffer> GetRLUniformBuffer() const;
		void AddObject(std::shared_ptr<Object> object, Bool needSerialization = True);
		void AddLight(std::shared_ptr<Object> object, Bool needSerialization = True);

		template <class Archive>
		void serialize(Archive & ar)
		{
			ar(serializedObjects);
		}

		~Scene();
	};
}