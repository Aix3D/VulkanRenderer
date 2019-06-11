#pragma once
#include "IAsset.h"

namespace Core
{
	class RawTexture : public IRawAsset
	{
	public:
		RawTexture();
		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual ~RawTexture();
	};
}