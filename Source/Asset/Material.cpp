#include "Material.h"

namespace Core
{
	Material::Material()
	{

	}

	int32 Material::GetFeatureID() const
	{
		//	TODO:
		//		根据材质特征返回特征ID,用以实现在特征相同的材质之间共享数据
		//		暂时都返回1

		return 1;
	}

	void Material::OnPause()
	{

	}

	void Material::OnResume()
	{

	}

	Material::~Material()
	{

	}
}