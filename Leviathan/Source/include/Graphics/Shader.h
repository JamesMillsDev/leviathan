#pragma once

#include "Maths/Color.h"

namespace Leviathan
{
	enum class EShaderStage : uint16
	{
		Vertex = 0x8B31,
		Fragment = 0x8B30,
		Geometry = 0x8DD9,
		TesselationControl = 0x8E88,
		TesselationEvaluation = 0x8E87,
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
