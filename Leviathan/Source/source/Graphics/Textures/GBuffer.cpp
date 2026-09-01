#include "Graphics/Textures/GBuffer.h"

#include <glad/gl.h>

#include "Graphics/Material.h"

namespace Leviathan
{
	uint32 GBuffer::GenerateTextureBuffer(const int32 w, const int32 h, const int32 format, const uint32 type, TList<uint32>& attachments)
	{
		uint32 handle;

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_RGBA, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<uint32>(attachments.Count()), GL_TEXTURE_2D, handle, 0);

		attachments.Add(GL_COLOR_ATTACHMENT0 + static_cast<uint32>(attachments.Count()));

		return handle;
	}

	GBuffer::GBuffer(const int32 w, const int32 h) :
		m_bound{ false }
	{
		// Generate the handle
		glGenFramebuffers(1, &m_handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

		// Build out the gBuffer elements
		TList<uint32> attachments;
		m_bufferHandles.Add("gBuffer.location", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments)); // Position
		m_bufferHandles.Add("gBuffer.normal", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments)); // Normal
		m_bufferHandles.Add("gBuffer.tangent", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments)); // Tangent
		m_bufferHandles.Add("gBuffer.biTangent", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments)); // Bitangent
		m_bufferHandles.Add("gBuffer.albedoSpec", GenerateTextureBuffer(w, h, GL_RGBA, GL_UNSIGNED_BYTE, attachments)); // Albedo + Specular

		// Assign the frame buffers and clear
		glDrawBuffers(static_cast<int32>(attachments.Count()), attachments.Data());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GBuffer::~GBuffer()
	{
		for (TMapEntry<string, uint32>* handle : m_bufferHandles)
		{
			uint32 texHandle = handle->Value();
			glDeleteTextures(1, &texHandle);
		}
		m_bufferHandles.Clear();

		glDeleteFramebuffers(1, &m_handle);
	}

	void GBuffer::BeginRecording()
	{
		// Validate the handle isn't 0
		if (m_handle == 0)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
		m_bound = true;
	}

	void GBuffer::FinishRecording()
	{
		if (!m_bound)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_bound = false;
	}

	void GBuffer::Bind(Material* material)
	{
		int index = 0;
		for (TMapEntry<string, uint32>* handle : m_bufferHandles)
		{
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, handle->Value());
			material->Set(handle->Key(), index);
			++index;
		}
	}
}
