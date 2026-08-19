#pragma once

#include <glad/gl.h>

#include "Maths/Color.h"

namespace Leviathan
{
	enum class EShaderStage : uint16
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Geometry = GL_GEOMETRY_SHADER,
		TesselationControl = GL_TESS_CONTROL_SHADER,
		TesselationEvaluation = GL_TESS_EVALUATION_SHADER,
	};

	class Shader
	{
		friend class Material;

	private:
		struct SubShader
		{
		public:
			uint32 handle;
			string source;
			bool compiled;

		public:
			explicit SubShader(EShaderStage type, const string& name);

		};

	private:
		uint32 m_handle;

	public:
		explicit Shader(const string& name);
		~Shader();

	private:
		bool Bind() const;

	};
}
