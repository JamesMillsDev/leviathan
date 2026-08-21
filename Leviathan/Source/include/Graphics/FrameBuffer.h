#pragma once

#include "Maths/Alias.h"

namespace Leviathan
{
	class FrameBuffer
	{
	private:
		uint32 m_handle;
		uint32 m_textureHandle;

		int32 m_width;
		int32 m_height;

		int32 m_component;
		uint32 m_attachment;
		uint32 m_format;
		int32 m_filter;
		int32 m_wrap;

	public:
		FrameBuffer(int32 w, int32 h, int32 component, uint32 attachment, uint32 format, int32 filter, int32 wrap);
		~FrameBuffer();

	public:
		void Bind() const;

		void Unbind();

		[[nodiscard]] uint32 Handle() const;
		[[nodiscard]] uint32 TextureHandle() const;
		[[nodiscard]] int32 Width() const;
		[[nodiscard]] int32 Height() const;

	};
}
