#include "Graphics/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/gl.h>

namespace Leviathan
{
	Texture::Texture() :
		m_fileName{ "none" }, m_width{ 0 }, m_height{ 0 }, m_channels{ 4 }, m_handle{ 0 }, m_pixels{ nullptr },
		m_stbiLoaded{ false }
	{}

	Texture::Texture(string fileName)
		: m_fileName{ std::move(fileName) }, m_handle{ 0 }, m_stbiLoaded{ true }
	{
		stbi_set_flip_vertically_on_load(true);
		m_pixels = stbi_load(m_fileName.c_str(), &m_width, &m_height, &m_channels, STBI_default);
		Load();
	}

	Texture::Texture(const int32 w, const int32 h, uint8* pixels)
		: m_fileName{ "none" }, m_width{ w }, m_height{ h }, m_channels{ 4 }, m_handle{ 0 }, m_pixels{ pixels },
		m_stbiLoaded{ false }
	{
		Load();
	}

	Texture::~Texture()
	{
		if (m_handle != 0)
		{
			glDeleteTextures(1, &m_handle);

			m_handle = 0;
			m_width = 0;
			m_height = 0;
			m_fileName = "none";
		}

		if (m_stbiLoaded)
		{
			stbi_image_free(m_pixels);
			m_pixels = nullptr;
			return;
		}

		delete[] m_pixels;
	}

	uint32 Texture::Handle() const
	{
		return m_handle;
	}

	void Texture::Load()
	{
		if (m_pixels == nullptr)
		{
			return;
		}

		glGenTextures(1, &m_handle);
		glBindTexture(GL_TEXTURE_2D, m_handle);

		switch (m_channels)
		{
			case 1:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, m_pixels);

				break;
			}

			case 2:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_width, m_height, 0, GL_RG, GL_UNSIGNED_BYTE, m_pixels);
				break;
			}

			case 3:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pixels);
				break;
			}

			case 4:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels);
				break;
			}
			default:
			{
				break;
			}
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
