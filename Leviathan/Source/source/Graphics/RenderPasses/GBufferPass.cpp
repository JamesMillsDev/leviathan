#include "Graphics/RenderPasses/GBufferPass.h"

#include <glad/gl.h>

#include <glm/mat3x3.hpp>

#include "Graphics/Resources/Material.h"
#include "Graphics/Resources/Mesh.h"

namespace Leviathan
{
	void GBufferPass::RenderFnc(const mat4& projection, const mat4& view, const vec3& viewLoc,
		const PassRenderInfo& info)
	{
		m_passMaterial->ClearMaterialProperties();
		m_passMaterial->CopyMaterialProperties(info.material);

		m_passMaterial->Set("pv", projection * view);
		m_passMaterial->Set("cameraLocation", viewLoc);

		m_passMaterial->Set("model", info.transform);
		m_passMaterial->Set("normMatrix", mat3(glm::transpose(glm::inverse(info.transform))));

		m_passMaterial->SetMaterialProperties();

		info.mesh->Render();
	}

	uint32 GBufferPass::ClearFlags()
	{
		return GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	}
}
