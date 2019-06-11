#pragma once
#include "IAsset.h"

namespace Core
{
	class RawShader : public IRawAsset
	{
	public:
		RawShader();
		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual ~RawShader();
	};
}