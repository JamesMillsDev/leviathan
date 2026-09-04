#include "Graphics/GBuffer.h"

#include <glad/gl.h>

#include "Core/Window.h"
#include "Graphics/Resources/Material.h"
#include "Graphics/Resources/Mesh.h"

namespace Leviathan
{
	GBuffer::GBufferHandle GBuffer::GenerateTextureBuffer(const int32 w, const int32 h, const int32 format, const uint32 type, TList<uint32>& attachments)
	{
		uint32 handle;

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_RGBA, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<uint32>(attachments.Count()), GL_TEXTURE_2D, handle, 0);

		attachments.Add(GL_COLOR_ATTACHMENT0 + static_cast<uint32>(attachments.Count()));

		return { .handle = handle };
	}

	GBuffer::GBufferHandle GBuffer::GenerateDepthAttachment(int32 w, int32 h)
	{
		uint32 handle;

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, handle, 0);

		return {.handle = handle, .render = false };
	}

	GBuffer::GBuffer(const shared_ptr<Window>& window) :
		m_bound{ false }, m_shader{ new Shader{ "Shaders/gbuffer" } }
	{
		m_material = new Material{ m_shader };

		// Generate the handle
		glGenFramebuffers(1, &m_handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

		const int32 w = window->Width(), h = window->Height();
		BuildBuffers(w, h);

		window->onWindowResized.Add([this](int32 w, int32 h)
			{
				for (TMapEntry<string, GBufferHandle>* handle : m_bufferHandles)
				{
					uint32 texHandle = handle->Value().handle;
					glDeleteTextures(1, &texHandle);
				}
				m_bufferHandles.Clear();

				glDeleteFramebuffers(1, &m_handle);

				BuildBuffers(w, h);
			});
	}

	GBuffer::~GBuffer()
	{
		delete m_material;
		delete m_shader;

		for (TMapEntry<string, GBufferHandle>* handle : m_bufferHandles)
		{
			uint32 texHandle = handle->Value().handle;
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

	void GBuffer::Render(const mat4& projection, const mat4& view, const vec3& viewLoc)
	{
		BeginRecording();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		while (!m_render.empty())
		{
			GBufferRender render = m_render.front();
			render.fnc(projection, view, viewLoc, render);

			m_render.pop();
		}

		FinishRecording();
	}

	void GBuffer::QueueRender(Material* material, Mesh* mesh, const mat4& transform)
	{
		m_render.push(GBufferRender
			{
				.material = material,
				.mesh = mesh,
				.transform = transform,
				.fnc = [this](const mat4& projection, const mat4& view, const vec3& viewLoc, const GBufferRender& render)
				{
					if (!m_material->Bind())
					{
						return;
					}

					m_material->CopyMaterialProperties(render.material);

					m_material->Set(m_material->m_vpLoc, projection * view);
					m_material->Set(m_material->m_cameraLocationLoc, viewLoc);

					m_material->Set(m_material->m_modelLoc, render.transform);
					m_material->Set(m_material->m_normalMatrixLoc, mat3(glm::transpose(glm::inverse(render.transform))));

					m_material->SetMaterialProperties(0, false, 0);

					render.mesh->Render();

					m_material->UnBind();
				}
			}
		);
	}

	int32 GBuffer::Bind(Material* material) 
	{
		int32 index = 0;
		for (TMapEntry<string, GBufferHandle>* handle : m_bufferHandles)
		{
			if (!handle->Value().render)
			{
				continue;
			} 

			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, handle->Value().handle);
			material->Set(handle->Key(), index);
			++index;
		}

		return index;
	}

	uint32 GBuffer::Handle() const
	{
		return m_handle;
	}

	void GBuffer::BuildBuffers(int32 w, int32 h)
	{
		// Generate the handle
		glGenFramebuffers(1, &m_handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

		// Build out the gBuffer elements
		TList<uint32> attachments;
		// Depth
		m_bufferHandles.Add("gBuffer.depth", GenerateDepthAttachment(w, h));
		// Position
		m_bufferHandles.Add("gBuffer.location", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments));
		// Normal
		m_bufferHandles.Add("gBuffer.normal", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments));
		// Tangent
		m_bufferHandles.Add("gBuffer.tangent", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments));
		// Albedo
		m_bufferHandles.Add("gBuffer.albedo", GenerateTextureBuffer(w, h, GL_RGBA, GL_UNSIGNED_BYTE, attachments));
		// Tint
		m_bufferHandles.Add("gBuffer.tint", GenerateTextureBuffer(w, h, GL_RGBA, GL_UNSIGNED_BYTE, attachments));
		// ORM
		m_bufferHandles.Add("gBuffer.orm", GenerateTextureBuffer(w, h, GL_RGBA, GL_UNSIGNED_BYTE, attachments));

		// Assign the frame buffers and clear
		glDrawBuffers(static_cast<int32>(attachments.Count()), attachments.Data());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
