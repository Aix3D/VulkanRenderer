#pragma once
#include "IAsset.h"
#include "..\Math\Vector4.h"
#include "Texture.h"
#include "Shader.h"

#include MAP_INCLUDE_PATH

namespace Core
{
	//struct MaterialProperites
	//{
	//	Vector4 ambient;
	//	Vector4 diffuse;
	//	Vector4 specular;
	//	float opacity;
	//};

	class Material : public IAsset
	{
	public:
		//MaterialProperites properites;
		std::weak_ptr<Texture> albedoTexture;
		std::weak_ptr<Shader> vertexShader;
		std::weak_ptr<Shader> fragmentShader;

		float albedoColor[4] = { 1.0f, 1.0f,1.0f,1.0f };
		float opacity;

		Material();
		int32 GetFeatureID() const;
		virtual void OnPause() override;
		virtual void OnResume() override;
		virtual ~Material();
	};
}