// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include "Graphics/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glad/gl.h>

namespace Leviathan
{
	Mesh::SubMesh::SubMesh()
		: m_vao{ 0 }, m_vbo{ 0 }, m_ibo{ 0 }, m_vertexCount{ 0 }, m_triangleCount{ 0 }, m_hasIndices{ false }
	{

	}

	Mesh::SubMesh::SubMesh(const TList<Vertex>& vertices, const TList<uint32>& indices)
		: m_vao{ 0 }, m_vbo{ 0 }, m_ibo{ 0 }, m_vertexCount{ vertices.Count() }, m_triangleCount{ indices.Count() },
		m_hasIndices{ !indices.IsEmpty() }
	{
		GenerateOglObjects(vertices, indices);
	}

	Mesh::SubMesh::~SubMesh()
	{
		CleanOglObjects();
	}

	void Mesh::SubMesh::GenerateOglObjects(const TList<Vertex>& vertices, const TList<uint32>& indices)
	{
		// If we have already generated the OpenGL objects, clean them for regeneration
		if (m_vao != 0)
		{
			CleanOglObjects();
		}

		// We can't generate the OpenGL objects if there is no vertex data
		if (vertices.IsEmpty())
		{
			return;
		}

		// Generate the vertex OpenGL objects.
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		uint32 locationIndex = 0;

		// Set the buffer data preparing for the attribute mapping
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.Count() * static_cast<int64>(sizeof(Vertex)), vertices.Data(), GL_STATIC_DRAW);

		// Generate the index buffer and assign it if there are any indices
		if (!indices.IsEmpty())
		{
			glGenBuffers(1, &m_ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.Count() * static_cast<int64>(sizeof(uint32)), indices.Data(), GL_STATIC_DRAW);
		}

		// Generate the attributes in the order -> Location, Normal, Tangent, UV0-7, Color0-7
		glVertexAttribPointer(locationIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(locationIndex++);

		glVertexAttribPointer(locationIndex, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(locationIndex++);

		glVertexAttribPointer(locationIndex, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
		glEnableVertexAttribArray(locationIndex++);

		for (uint8 i = 0; i < MAX_UV_CHANNELS; ++i)
		{
			glVertexAttribPointer(locationIndex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uvs[i])));
			glEnableVertexAttribArray(locationIndex++);
		}

		glVertexAttribPointer(locationIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
		glEnableVertexAttribArray(locationIndex);

		// Clear the bound array
		glBindVertexArray(0);
	}

	void Mesh::SubMesh::CleanOglObjects()
	{
		// Delete the old vertex OpenGL objects
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);

		// If there were any indices, delete the IBO.
		if (m_hasIndices)
		{
			glDeleteBuffers(1, &m_ibo);
		}

		m_vao = 0;
		m_vbo = 0;
		m_ibo = 0;
	}

	Mesh* Mesh::MakeFromAssimp(const string& path)
	{
		// Attempt to import the asset
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path.c_str(), aiProcess_Triangulate | aiProcess_GlobalScale | aiProcess_CalcTangentSpace
		);

		TList<SubMesh> subMeshes;

		// Iterate over each mesh 
		for (uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			const aiMesh* mesh = scene->mMeshes[meshIndex];

			// Iterate over each vertex in the mesh
			TList<Vertex> vertices;
			vertices.Resize(mesh->mNumVertices);
			for (uint32 vertIndex = 0; vertIndex < mesh->mNumVertices; ++vertIndex)
			{
				Vertex vert;

				// Copy the vertex location across
				aiVector3D location = mesh->mVertices[vertIndex];
				vert.location = { location.x, location.y, location.z, 1.f };

				// If there are any normals, copy them across
				if (mesh->HasNormals())
				{
					aiVector3D normal = mesh->mNormals[vertIndex];

					vert.normal = { normal.x, normal.y, normal.z, 0.f };
				}

				// If there are any tangents, copy them across
				if (mesh->HasTangentsAndBitangents())
				{
					aiVector3D tangent = mesh->mTangents[vertIndex];

					vert.tangent = { tangent.x, tangent.y, tangent.z, 0.f };
				}

				// Iterate over each possible UV channel
				for (uint8 i = 0; i < MAX_UV_CHANNELS; i++)
				{
					// If a set exists for this index, copy it
					if (mesh->HasTextureCoords(i))
					{
						aiVector3D uv = mesh->mTextureCoords[i][vertIndex];

						vert.uvs[i] = { uv.x, uv.y };
					}
				}

				// If a set exists for this index, copy it
				if (mesh->HasVertexColors(0))
				{
					aiColor4D color0 = mesh->mColors[0][vertIndex];

					vert.color = Color{ color0.r, color0.g, color0.b, color0.a };
				}
				else
				{
					// Otherwise, make it white
					vert.color = Color{ 1.f, 1.f, 1.f, 1.f };
				}

				vertices[vertIndex] = vert;
			}

			// Iterate over each face in the mesh
			TList<uint32> indices;
			for (uint32 faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
			{
				const aiFace face = mesh->mFaces[faceIndex];

				indices.Add(face.mIndices[1]);
				indices.Add(face.mIndices[2]);
				indices.Add(face.mIndices[0]);

				// Generate a second triangle for quads
				if (face.mNumIndices == 4)
				{
					indices.Add(face.mIndices[2]);
					indices.Add(face.mIndices[3]);
					indices.Add(face.mIndices[0]);
				}
			}

			// Add the built submesh
			subMeshes.Add(SubMesh{ vertices, indices });
		}

		// Set the submesh data and return the mesh;
		Mesh* newMesh = new Mesh;
		newMesh->m_submeshes = subMeshes;
		return newMesh;
	}

	Mesh::Mesh() = default;

	Mesh::~Mesh()
	{
		for (SubMesh& subMesh : m_submeshes)
		{
			subMesh.CleanOglObjects();
		}
	}

	void Mesh::Render()
	{
		for (const SubMesh& subMesh : m_submeshes)
		{
			glBindVertexArray(subMesh.m_vao);
			if (subMesh.m_hasIndices)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.m_ibo);
				glDrawElements(GL_TRIANGLES, static_cast<int32>(subMesh.m_triangleCount), GL_UNSIGNED_INT, nullptr);
			}
			else
			{
				glDrawArrays(GL_TRIANGLES, 0, static_cast<int32>(subMesh.m_vertexCount));
			}
		}
	}
}
