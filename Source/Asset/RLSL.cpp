#include "RLSL.h"
#include "..\Helper\FileOperator.h"

namespace Core
{

	void RLSL::load()
	{
		ANSICHAR * pMem = (ANSICHAR *)FileOperator::MapMemory(fullPathName.c_str());

		if (pMem == Null)
			return;

		if (type == FileType_RLSL_Vertex)
		{
			if (m_shader.get() == Null)
				m_shader = std::make_unique<RLShader>(RLShaderType_VertexShader);
		}
		else if (type == FileType_RLSL_Ray)
		{
			if (m_shader.get() == Null)
				m_shader = std::make_unique<RLShader>(RLShaderType_RayShader);
		}
		else if (type == FileType_RLSL_Frame)
		{
			if (m_shader.get() == Null)
				m_shader = std::make_unique<RLShader>(RLShaderType_FrameShader);
		}
		else
			assert(False);

		ErrorCode result = m_shader->Compile(pMem);

		if (result != ErrorCode_OK)
			return;

		FileOperator::UnmapMemory(pMem);
	}

	RLSL::RLSL()
	{

	}

	void RLSL::BeginUse()
	{
		if (!m_beingUsed)
		{
			load();
			m_beingUsed = True;
		}
	}

	void RLSL::Reload()
	{
		NotifyAhead();
		load();
		NotifyBehind();
	}

	void RLSL::BeforeSave()
	{
	}

	void RLSL::AfterLoad()
	{

	}

	RLShader * RLSL::GetRLShader() const
	{
		return m_shader.get();
	}

	RLSL::~RLSL()
	{

	}
}