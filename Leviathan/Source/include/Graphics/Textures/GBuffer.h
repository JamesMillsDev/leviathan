#pragma once

#include "Utility/Collections/TList.h"

namespace Leviathan
{
	class GBuffer
	{
		friend class Renderer;

	private:
		static uint32 GenerateTextureBuffer(int32 w, int32 h, int32 format, uint32 type, TList<uint32>& attachments);

	private:
		uint32 m_handle;
		bool m_bound;

		TList<uint32> m_bufferHandles;

	private:
		GBuffer(int32 w, int32 h);
		~GBuffer();

	private:
		void Bind();
		void Unbind();

		void BindBuffers();
		void Render();

	};
}
