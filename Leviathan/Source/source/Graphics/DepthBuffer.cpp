#include "Graphics/DepthBuffer.h"

#include <glad/gl.h>

#include "Core/Window.h"

#include "Graphics/Resources/Material.h"
#include "Graphics/Resources/Mesh.h"
#include "Graphics/Resources/Shader.h"
#include "Graphics/Resources/Textures/FrameBuffer.h"

namespace Leviathan
{
	DepthBuffer::DepthBuffer(const shared_ptr<Window>& window) :
		m_window{ window }, m_bound{ false }, m_shader{ new Shader{ "Shaders/depth" } }
	{
		m_material = new Material{ m_shader };

		m_frameBuffer = new FrameBuffer{
			m_window->Width(), m_window->Height(), GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_FLOAT,
			GL_NEAREST, GL_CLAMP_TO_BORDER
		};

		window->onWindowResized.Add([this](int32 w, int32 h)
			{
				m_frameBuffer->Resize(w, h);
			});
	}

	DepthBuffer::~DepthBuffer()
	{
		delete m_shader;
		delete m_material;
		delete m_frameBuffer;
	}

	void DepthBuffer::BeginRecording()
	{
		if (m_bound)
		{
			return;
		}

		m_frameBuffer->Bind();
		m_bound = true;
	}

	void DepthBuffer::QueueRender(Mesh* mesh, const mat4& transform)
	{
		m_renders.push([this, transform, mesh](const mat4& projection, const mat4& view)
			{
				if (!m_material->Bind())
				{
					return;
				}

				m_material->Set(m_material->m_vpLoc, projection * view);
				m_material->Set(m_material->m_modelLoc, transform);

				mesh->Render();
			});
	}

	void DepthBuffer::Render(const mat4& projection, const mat4& view)
	{
		BeginRecording();
		m_window->Clear(GL_DEPTH_BUFFER_BIT);

		while (!m_renders.empty())
		{
			m_renders.front()(projection, view);
			m_renders.pop();
		}

		FinishRecording();
	}

	void DepthBuffer::FinishRecording()
	{
		if (!m_bound)
		{
			return;
		}

		m_bound = false;
		m_frameBuffer->Unbind();
	}
}
