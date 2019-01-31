#pragma once
#include <BaseInclude.h>
#include "IAsset.h"

namespace Core
{
	class FBX : public IAsset
	{
	public:
		FBX();

		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual void BeforeSave() override;
		virtual void AfterLoad() override;

		virtual ~FBX();
	};
}