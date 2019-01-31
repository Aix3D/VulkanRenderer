#include "Texture.h"
#include "..\Helper\TextureOperator.h"

namespace Core
{
	void Texture::load()
	{
		if (pImage != Null)
			delete[] pImage;

		TextureOperator::LoadTexture(fullPathName.c_str(), info->format, this);
	}

	Texture::Texture()
		:pImage(Null)
	{

	}

	void Texture::BeginUse()
	{
		if (!m_beingUsed)
		{
			load();
			UploadToGL();
			UploadToRL();
	
			m_beingUsed = True;
		}
	}

	void Texture::Reload()
	{
		load();
		UploadToGL();
		UploadToRL();
	}

	void Texture::BeforeSave()
	{
	}

	void Texture::AfterLoad()
	{
	}

	void Texture::Activate()
	{
		m_glTexture->Activate();
	}

	void Texture::Inactivate()
	{
		m_glTexture->Inactivate();
	}

	RLTexture2D * Texture::GetRLTexture()
	{
		return m_rlTexture.get();
	}

	void Texture::SetGLWrapMode(GLTextureWrapMode wrapMode)
	{
		m_glTexture->SetWrapMode(wrapMode);
	}

	Core::GLTextureWrapMode Texture::GetGLWrapMode() const
	{
		return m_glTexture->GetWrapMode();
	}

	void Texture::SetGLFilterMode(GLTextureFilterMode filterMode)
	{
		m_glTexture->SetFilterMode(filterMode);
	}

	Core::GLTextureFilterMode Texture::GetGLFilterMode() const
	{
		return m_glTexture->GetFilterMode();
	}

	void Texture::SetRLWrapMode(RLTextureWrapMode wrapMode)
	{
		m_rlTexture->SetWrapMode(wrapMode);
	}

	Core::RLTextureWrapMode Texture::GetRLWrapMode() const
	{
		return m_rlTexture->GetWrapMode();
	}

	void Texture::SetRLFilterMode(RLTextureFilterMode filterMode)
	{
		m_rlTexture->SetFilterMode(filterMode);
	}

	Core::RLTextureFilterMode Texture::GetRLFilterMode() const
	{
		return m_rlTexture->GetFilterMode();
	}

	void Texture::UploadToGL()
	{
		GLInternalFormat glInternalFormat;
		GLPixelFormat glPixelFormat;

		switch (info->format)
		{
		case TextureFormat_RGB24:
			glInternalFormat = GLInternalFormat_RGB;
			glPixelFormat = GLPixelFormat_RGB;
			break;
		case TextureFormat_RGBA32:
			glInternalFormat = GLInternalFormat_RGBA;
			glPixelFormat = GLPixelFormat_RGBA;
			break;
		case TextureFormat_SRGB24:
			glInternalFormat = GLInternalFormat_SRGB8;
			glPixelFormat = GLPixelFormat_RGB;
			break;
		case TextureFormat_SRGBA32:
			glInternalFormat = GLInternalFormat_SRGB8_ALPHA8;
			glPixelFormat = GLPixelFormat_RGBA;
			break;
		default:
			assert(False);
			break;
		}

		m_glTexture = std::make_unique<GLTexture>(GLTextureTarget_2D, glInternalFormat, glPixelFormat, GLDataType_UnsignedByte, GLTextureWrapMode_Clamp, GLTextureFilterMode_Point);

		m_glTexture->LoadImage(
			width,
			height,
			pImage);

		switch (info->wrapMode)
		{
		case TextureWrapMode_Repeat:
			SetGLWrapMode(GLTextureWrapMode_Repeat);
			break;
		case TextureWrapMode_Clamp:
			SetGLWrapMode(GLTextureWrapMode_Clamp);
			break;
		default:
			break;
		}

		switch (info->filterMode)
		{
		case TextureFilterMode_Point:
			SetGLFilterMode(GLTextureFilterMode_Point);
			break;
		case TextureFilterMode_Bilinear:
			SetGLFilterMode(GLTextureFilterMode_Bilinear);
			break;
		default:
			break;
		}
	}

	void Texture::UploadToRL()
	{
		RLIinternalFormat rlInternalFormat;
		RLPixelFormat rlPixelFormat;

		switch (info->format)
		{
		case TextureFormat_RGB24:
			rlInternalFormat = RLIinternalFormat_RGB;
			rlPixelFormat = RLPixelFormat_RGB;
			break;
		case TextureFormat_RGBA32:
			rlInternalFormat = RLIinternalFormat_RGBA;
			rlPixelFormat = RLPixelFormat_RGBA;
			break;
		case TextureFormat_SRGB24:
			rlInternalFormat = RLIinternalFormat_RGB;
			rlPixelFormat = RLPixelFormat_RGB;
			break;
		case TextureFormat_SRGBA32:
			rlInternalFormat = RLIinternalFormat_RGBA;
			rlPixelFormat = RLPixelFormat_RGBA;
			break;
		default:
			assert(False);
			break;
		}

		m_rlTexture = std::make_unique<RLTexture2D>(rlInternalFormat, rlPixelFormat, RLDataType_UnsignedByte, RLTextureWrapMode_Clamp, RLTextureFilterMode_Point);

		m_rlTexture->LoadImage(width, height, pImage);

		switch (info->wrapMode)
		{
		case TextureWrapMode_Repeat:
			SetRLWrapMode(RLTextureWrapMode_Repeat);
			break;
		case TextureWrapMode_Clamp:
			SetRLWrapMode(RLTextureWrapMode_Clamp);
			break;
		default:
			break;
		}

		switch (info->filterMode)
		{
		case TextureFilterMode_Point:
			SetRLFilterMode(RLTextureFilterMode_Point);
			break;
		case TextureFilterMode_Bilinear:
			SetRLFilterMode(RLTextureFilterMode_Bilinear);
			break;
		default:
			break;
		}
	}

	Texture::~Texture()
	{
		if (pImage)
			delete[] pImage;
	}
}