#pragma once

#include "Graphics/RenderPasses/IRenderPass.h"

namespace Leviathan
{
	class GBufferPass : public IRenderPass
	{
	protected:
		void RenderFnc(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& viewLoc, const PassRenderInfo& info) override;
		uint32 ClearFlags() override;

	};
}