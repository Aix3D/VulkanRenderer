#pragma once
#include <BaseInclude.h>
#include "IAsset.h"
#include "../Component/TransformData.h"
#include "../Component/CameraData.h"
#include "../Component/RenderableEntityData.h"

namespace Core
{
	class Entity : public IAsset
	{
	private:
		static int32 idSeed;
		int32 m_id;

	public:
		//	All components may be used by entity listd here, it's so straightforward.
		std::unique_ptr<TransformData> transformData;
		std::unique_ptr<CameraData> cameraData;
		std::unique_ptr<RenderableEntityData> renderData;

		Entity();
		int32 GetID() const;
		virtual void OnPause() override;
		virtual void OnResume() override;
		~Entity();
	};
}