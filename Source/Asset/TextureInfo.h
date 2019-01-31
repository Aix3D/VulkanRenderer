#pragma once
#include <BaseInclude.h>
#include <../3rdParty/cereal/archives/xml.hpp>
#include "IAsset.h"

namespace Core
{
	enum TextureWrapMode
	{
		TextureWrapMode_Repeat,
		TextureWrapMode_Clamp,
	};

	enum TextureFilterMode
	{
		TextureFilterMode_Point,
		TextureFilterMode_Bilinear,
	};

	enum TextureFormat
	{
		TextureFormat_Unknown = -1,
		TextureFormat_RGB24,
		TextureFormat_RGBA32,
		TextureFormat_SRGB24,
		TextureFormat_SRGBA32,
		TextureFormat_Count,
	};

	class TextureInfo : public IAsset
	{
	public:
		TextureWrapMode wrapMode;
		TextureFilterMode filterMode;
		TextureFormat format;

		TextureInfo();

		virtual void BeginUse() override;
		virtual void Reload() override;
		virtual void BeforeSave() override;
		virtual void AfterLoad() override;

		template <class Archive>
		void serialize(Archive & ar)
		{
			ar(wrapMode,
				filterMode,
				format
			);
		}

		~TextureInfo();
	};
}