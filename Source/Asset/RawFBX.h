#pragma once
#include "IAsset.h"

namespace Core
{
	class RawFBX : public IRawAsset
	{
	public:
		RawFBX();
		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual ~RawFBX();
	};
}