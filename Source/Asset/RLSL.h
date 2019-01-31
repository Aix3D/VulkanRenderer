#pragma once
#include <BaseInclude.h>
#include "IAsset.h"
#include "..\RHI\OpenRLRHI\RLShader.h"
#include "..\DesignPattern\ObserverPattern\Subject.h"

namespace Core
{
	class RLSL : public IAsset, public Subject
	{
	private:
		std::unique_ptr<RLShader> m_shader;
		void load();
	public:
		RLSL();
		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual void BeforeSave() override;
		virtual void AfterLoad() override;
		RLShader * GetRLShader() const;
		virtual ~RLSL();
	};
}