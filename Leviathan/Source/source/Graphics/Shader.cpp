#include "Graphics/Shader.h"

#include <format>
#include <fstream>
#include <unordered_map>
#include <glad/gl.h>

#include "Utility/Console.h"
#include "Utility/Collections/TList.h"

using std::ifstream;
using std::istreambuf_iterator;
using std::unordered_map;

using Leviathan::EShaderStage;

namespace
{
	constexpr uint16 INFO_LOG_LENGTH = 512;

	unordered_map<EShaderStage, string> fileExtensions =
	{
		{ EShaderStage::Vertex, "vert" },
		{ EShaderStage::Fragment, "frag" },
		{ EShaderStage::Geometry, "geom" },
		{ EShaderStage::TesselationControl, "tessctrl" },
		{ EShaderStage::TesselationEvaluation, "tesseval" },
	};
}

namespace Leviathan
{
	Shader::SubShader::SubShader(const EShaderStage type, const string& name)
		: handle{ 0 }, compiled{ true }
	{
		if (ifstream file{ std::format("Content/{}.{}", name, fileExtensions[type]) }; file.is_open())
		{
			// Read all shader source code and generate the shader handle
			source = string{ istreambuf_iterator(file), istreambuf_iterator<char>() };
			handle = glCreateShader(static_cast<int32>(type));

			// Attempt to compile the shader
			const char* src = source.c_str();
			glShaderSource(handle, 1, &src, nullptr);
			glCompileShader(handle);

			// Check the compilation status
			int32 success;
			char infoLog[INFO_LOG_LENGTH];
			glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				// Get the error message
				glGetShaderInfoLog(handle, INFO_LOG_LENGTH, nullptr, infoLog);
				Console::Error(std::format("SHADER STAGE {} FAILED TO COMPILE!", static_cast<int32>(type))); 
				compiled = false;
			} 

			file.close();
			return;
		}

		compiled = false;
	}

	Shader::Shader(const string& name)
		: m_handle{ glCreateProgram() }
	{
		bool hasAnyLoaded = false;

		TList<SubShader*> shaders;
		for (const EShaderStage stage : { EShaderStage::Vertex, EShaderStage::Fragment, EShaderStage::Geometry, EShaderStage::TesselationControl, EShaderStage::TesselationEvaluation })
		{
			// Attempt to generate the sub shader
			SubShader* subShader = new SubShader{ stage, name };
			if (!subShader->compiled)
			{
				delete subShader;
				continue;
			}

			// We successfully compiled the shader, so attach it and store it (for deletion)
			hasAnyLoaded = true;
			glAttachShader(m_handle, subShader->handle);
			shaders.Add(subShader);
		}

		// If no shaders successfully compiled, delete the program
		if (!hasAnyLoaded)
		{
			glDeleteProgram(m_handle);
			return;
		}

		// Attempt to link the program
		glLinkProgram(m_handle);

		// Check that the program did link
		int32 success;
		char infoLog[INFO_LOG_LENGTH];
		glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
		if (!success)
		{
			// Log out the error
			glGetProgramInfoLog(m_handle, INFO_LOG_LENGTH, nullptr, infoLog);
			Console::Error(std::format("SHADER {} FAILED TO LINK!", name));
		}

		// Iterate over each shader and delete them
		for (const SubShader* subShader : shaders)
		{
			delete subShader;
		}
	}

	Shader::~Shader()
	{
		if (m_handle != 0)
		{
			glDeleteProgram(m_handle);
		}
	}

	bool Shader::Bind() const
	{
		if (m_handle == 0)
		{
			return false;
		}

		glUseProgram(m_handle);
		return true;
	}
}
