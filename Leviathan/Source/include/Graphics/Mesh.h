#pragma once

#include "Maths/Alias.h"
#include "Maths/Color.h"

#include "Utility/Collections/TList.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Utility/Collections/TArray.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace Leviathan
{
	constexpr uint8 MAX_UV_CHANNELS = 8;

	struct Vertex
	{
		vec4 location;
		vec4 normal;
		vec4 tangent;
		vec2 uvs[MAX_UV_CHANNELS];
		Color color;
	};

	class Mesh
	{
		friend class GBuffer;
		friend class Lighting;
		friend class Renderer;

	public:
		class SubMesh
		{
			friend Mesh;

		private:
			uint32 m_vao;
			uint32 m_vbo;
			uint32 m_ibo;
			int64 m_vertexCount;
			int64 m_triangleCount;

			bool m_hasIndices;

		public:
			SubMesh();
			SubMesh(const TList<Vertex>& vertices, const TList<uint32>& indices);

			~SubMesh();

		private:
			void GenerateOglObjects(const TList<Vertex>& vertices, const TList<uint32>& indices);
			void CleanOglObjects();

		};

	public:
		static Mesh* MakeFromAssimp(const string& path);
		static Mesh* MakeCube();
		static Mesh* MakePlane(vec3* normal = nullptr, TArray<vec2, 4>* uvs = nullptr);

	private:
		TList<SubMesh*> m_submeshes;

	public:
		Mesh();
		~Mesh();

	private:
		void Render();

	};
}