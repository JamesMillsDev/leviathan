#include "Graphics/GBuffer.h"

#include <glad/gl.h>

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"

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
		m_bound{ false }, m_shader{ new Shader{ "Shaders/gbuffer" } }
	{
		m_material = new Material{ m_shader };

		// Generate the handle
		glGenFramebuffers(1, &m_handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

		// Build out the gBuffer elements
		TList<uint32> attachments;
		// Position
		m_bufferHandles.Add("gBuffer.location", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments));
		// Normal
		m_bufferHandles.Add("gBuffer.normal", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments));
		// Tangent
		m_bufferHandles.Add("gBuffer.tangent", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments));
		// Bitangent
		m_bufferHandles.Add("gBuffer.biTangent", GenerateTextureBuffer(w, h, GL_RGBA16F, GL_FLOAT, attachments));
		// Albedo + Specular
		m_bufferHandles.Add("gBuffer.albedoSpec", GenerateTextureBuffer(w, h, GL_RGBA, GL_UNSIGNED_BYTE, attachments));

		// Assign the frame buffers and clear
		glDrawBuffers(static_cast<int32>(attachments.Count()), attachments.Data());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GBuffer::~GBuffer()
	{
		delete m_material;
		delete m_shader;

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

	void GBuffer::QueueForRender(Material* material, Mesh* mesh, const mat4& transform)
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

					m_material->SetMaterialProperties(0, false);

					render.mesh->Render();
				}
			}
		);
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

	uint32 GBuffer::Handle() const
	{
		return m_handle;
	}
}
