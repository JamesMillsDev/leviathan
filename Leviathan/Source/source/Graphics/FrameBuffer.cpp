#include "Graphics/FrameBuffer.h"

#include <glad/gl.h>

namespace Leviathan
{
	FrameBuffer::FrameBuffer(const int32 w, const int32 h, const int32 component, const uint32 attachment,
		const uint32 format, const int32 filter, const int32 wrap)
		: m_width{ w }, m_height{ h }, m_component{ component }, m_attachment{ attachment }, m_format{ format },
		m_filter{ filter }, m_wrap{ wrap }
	{
		glGenFramebuffers(1, &m_handle);

		glGenTextures(1, &m_textureHandle);
		glBindTexture(GL_TEXTURE_2D, m_textureHandle);

		glTexImage2D(GL_TEXTURE_2D, 0, m_component, m_width, m_height, 0, m_component, m_format, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap);

		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, m_attachment, GL_TEXTURE_2D, m_textureHandle, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		Unbind();
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteTextures(1, &m_textureHandle);

		glDeleteFramebuffers(1, &m_handle);
	}

	void FrameBuffer::Bind() const
	{
		if (m_handle == 0)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32 FrameBuffer::Handle() const
	{
		return m_handle;
	}

	uint32 FrameBuffer::TextureHandle() const
	{
		return m_textureHandle;
	}

	int32 FrameBuffer::Width() const
	{
		return m_width;
	}

	int32 FrameBuffer::Height() const
	{
		return m_height;
	}
}
