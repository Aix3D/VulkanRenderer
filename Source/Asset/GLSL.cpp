#include "GLSL.h"
#include "..\Helper\FileOperator.h"

namespace Core
{
	void GLSL::load()
	{
		ANSICHAR * pMem = (ANSICHAR *)FileOperator::MapMemory(fullPathName.c_str());

		if (pMem == Null)
			return;

		if (type == FileType_GLSL_Vertex)
		{
			if (m_shader.get() == Null)
				m_shader = std::make_unique<GLShader>(GLShaderType_VertexShader);
		}
		else if (type == FileType_GLSL_Fragment)
		{
			if (m_shader.get() == Null)
				m_shader = std::make_unique<GLShader>(GLShaderType_FragmentShader);
		}
		else
			assert(False);

		ErrorCode result = m_shader->Compile(pMem);

		if (result != ErrorCode_OK)
			return;

		FileOperator::UnmapMemory(pMem);
	}

	GLSL::GLSL()
	{
	}

	void GLSL::BeginUse()
	{
		if (!m_beingUsed)
		{
			load();
			m_beingUsed = True;
		}
	}

	void GLSL::Reload()
	{
		NotifyAhead();
		load();
		NotifyBehind();
	}

	void GLSL::BeforeSave()
	{

	}

	void GLSL::AfterLoad()
	{

	}

	GLShader * GLSL::GetGLShader() const
	{
		return m_shader.get();
	}

	GLSL::~GLSL()
	{

	}
}