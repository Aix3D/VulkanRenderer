#pragma once
#include "IAsset.h"

namespace Core
{
	enum ShaderType
	{
		ShaderType_Invalid = -1,
		ShaderType_Vertex,
		ShaderType_Geometry,
		ShaderType_Fragment,
		ShaderType_Count,
	};

	class Shader : public IAsset
	{
	private:

	public:
		ShaderType type;
		std::unique_ptr <ANSICHAR[]> pRawData;
		size_t size;

		Shader();
		virtual void OnPause();
		virtual void OnResume();
		virtual ~Shader();
	};
}