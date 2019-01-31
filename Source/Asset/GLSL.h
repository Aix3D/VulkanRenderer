#pragma once
#include <BaseInclude.h>
#include "IAsset.h"
#include "..\RHI\OpenGLRHI\GLShader.h"
#include "..\DesignPattern\ObserverPattern\Subject.h"

namespace Core
{
	class GLSL : public IAsset, public Subject
	{
	private:
		std::unique_ptr<GLShader> m_shader;
		void load();
	public:
		GLSL();
		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual void BeforeSave() override;
		virtual void AfterLoad() override;
		GLShader * GetGLShader() const;
		virtual ~GLSL();
	};
} 