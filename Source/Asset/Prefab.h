#pragma once
#include <BaseInclude.h>
#include <../3rdParty/cereal/archives/xml.hpp>
#include "IAsset.h"

#include STRING_INCLUDE_PATH

namespace Core
{
	class Prefab : public IAsset
	{
	public:
		ctd::string staticMeshName;
		ctd::string materialName;
		//Bool stitched;

		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual void BeforeSave() override;
		virtual void AfterLoad() override;

		template <class Archive>
		void serialize(Archive & ar)
		{
			ar(
				staticMeshName,
				materialName
				//stitched
			);
		}
	};
}