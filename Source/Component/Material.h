#pragma once
#include <BaseInclude.h>
#include <Vector4.h>

#include STRING_INCLUDE_PATH

namespace Core
{
	struct MaterialProperites
	{
		Vector4 ambient;
		Vector4 diffuse;
		Vector4 specular;
		float opacity;
	};

	struct Material
	{
		ctd::string name;
		MaterialProperites properites;
	};
}