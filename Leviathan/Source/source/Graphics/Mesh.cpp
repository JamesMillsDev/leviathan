// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include "Graphics/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/gl.h>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Utility/Collections/TArray.h"

using glm::mat4;

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
			glVertexAttribPointer(locationIndex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uvs) + i * sizeof(vec2)));
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
			("Content/" + path).c_str(), aiProcess_Triangulate | aiProcess_GlobalScale | aiProcess_CalcTangentSpace
		); 

		TList<SubMesh*> subMeshes;

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
					aiVector3D bitangent = mesh->mBitangents[vertIndex];
					aiVector3D normal = mesh->mNormals[vertIndex];

					// handedness: does the assimp-provided bitangent agree with cross(N, T)?
					float handedness = (normal ^ tangent) * bitangent < 0.f ? -1.f : 1.f;

					vert.tangent = { tangent.x, tangent.y, tangent.z, handedness }; 
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
			subMeshes.Add(new SubMesh{ vertices, indices });
		}

		// Set the submesh data and return the mesh;
		Mesh* newMesh = new Mesh;
		newMesh->m_submeshes = subMeshes;
		return newMesh;
	}

	Mesh* Mesh::MakeCube()
	{
		TList<Vertex> vertices;
		TList<uint32> indices;

		TArray directions =
		{
			vec4{ 0.f, 1.f, 0.f, 0.f },
			vec4{ 0.f, -1.f, 0.f, 0.f },
			vec4{ 1.f, 0.f, 0.f, 0.f },
			vec4{ -1.f, 0.f, 0.f, 0.f },
			vec4{ 0.f, 0.f, 1.f, 0.f },
			vec4{ 0.f, 0.f, -1.f, 0.f },
		};

		TArray points =
		{
			vec4{ .5f, .5f, -.5f, 1.f },
			vec4{ .5f, .5f, .5f, 1.f },
			vec4{ -.5f, .5f, .5f, 1.f },
			vec4{ -.5f, .5f, -.5f, 1.f },
		};

		TArray uvs =
		{
			vec2{ 0.f, 0.f },
			vec2{ 0.f, 1.f },
			vec2{ 1.f, 1.f },
			vec2{ 1.f, 0.f },
		};

		const vec2 deltaUV1 = uvs[1] - uvs[0];
		const vec2 deltaUV2 = uvs[2] - uvs[0];
		const float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		for (uint64 i = 0; i < directions.Count(); ++i)
		{
			const mat4 orientation = glm::mat4_cast(glm::rotation(vec3{ 0.f, 1.f, 0.f }, vec3{ directions[i] }));

			const vec4 edge1 = orientation * points[1] - orientation * points[0];
			const vec4 edge2 = orientation * points[2] - orientation * points[0];

			for (uint64 j = 0; j < points.Count(); ++j)
			{
				Vertex vert =
				{
					.location = orientation * points[j],
					.normal = directions[i],
					.tangent = {},
					.uvs = {uvs[j]},
					.color = Color::WHITE
				};

				vert.tangent =
				{
					f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
					f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
					f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z),
					0.f
				};

				vertices.Add(vert);
			}
		}

		indices.AddRange({ 0,  3,  1,  1,  3,  2 });
		indices.AddRange({ 4,  7,  5,  5,  7,  6 });
		indices.AddRange({ 8, 11,  9,  9, 11, 10 });
		indices.AddRange({ 12, 15, 13, 13, 15, 14 });
		indices.AddRange({ 16, 19, 17, 17, 19, 18 });
		indices.AddRange({ 20, 23, 21, 21, 23, 22 });

		TList<SubMesh*> subMeshes;
		subMeshes.Add(new SubMesh{ vertices, indices });

		Mesh* mesh = new Mesh;
		mesh->m_submeshes = subMeshes;

		return mesh;
	}

	Mesh* Mesh::MakePlane(vec3* normal, TArray<vec2, 4>* uvs)
	{
		const vec3 up = glm::normalize(normal == nullptr ? vec3{ 0.f, 1.f, 0.f } : *normal);
		const vec3 helper = std::abs(up.y) > .99f ? vec3{ 1.f, 0.f, 0.f } : vec3{ 0.f, 1.f, 0.f };

		const vec3 right = glm::normalize(glm::cross(helper, up));
		const vec3 forward = glm::normalize(glm::cross(up, right));

		TArray points =
		{
			vec4{ -forward + right, 1.f },
			vec4{ -forward - right, 1.f },
			vec4{ forward - right, 1.f },
			vec4{ forward + right, 1.f },
		};

		TArray uv = uvs != nullptr ? *uvs : TArray
		{
			vec2{ 0.f, 1.f },
			vec2{ 0.f, 0.f },
			vec2{ 1.f, 0.f },
			vec2{ 1.f, 1.f },
		};

		TArray<vec4, 2> tangents;

		const vec4 norm = vec4{ up, 0.f };
		vec4 edge1 = points[1] - points[0];
		vec4 edge2 = points[2] - points[0];

		vec2 deltaUv1 = uv[1] - uv[0];
		vec2 deltaUv2 = uv[2] - uv[0];

		float f = 1.f / (deltaUv1.x * deltaUv2.y - deltaUv2.x * deltaUv1.y);

		tangents[0] = vec4
		{
			f * (deltaUv2.y * edge1.x - deltaUv1.y * edge2.x),
			f * (deltaUv2.y * edge1.y - deltaUv1.y * edge2.y),
			f * (deltaUv2.y * edge1.z - deltaUv1.y * edge2.z),
			1.f
		};

		edge1 = points[2] - points[0];
		edge2 = points[3] - points[0];

		deltaUv1 = uv[2] - uv[0];
		deltaUv2 = uv[3] - uv[0];

		f = 1.f / (deltaUv1.x * deltaUv2.y - deltaUv2.x * deltaUv1.y);

		tangents[1] = vec4
		{
			f * (deltaUv2.y * edge1.x - deltaUv1.y * edge2.x),
			f * (deltaUv2.y * edge1.y - deltaUv1.y * edge2.y),
			f * (deltaUv2.y * edge1.z - deltaUv1.y * edge2.z),
			1.f
		};

		TList vertices =
		{
			Vertex{.location = points[2], .normal = norm, .tangent = tangents[0], .uvs = { uv[2] }, .color = Color::WHITE },
			Vertex{.location = points[1], .normal = norm, .tangent = tangents[0], .uvs = { uv[1] }, .color = Color::WHITE },
			Vertex{.location = points[0], .normal = norm, .tangent = tangents[0], .uvs = { uv[0] }, .color = Color::WHITE },
			Vertex{.location = points[3], .normal = norm, .tangent = tangents[1], .uvs = { uv[3] }, .color = Color::WHITE },
			Vertex{.location = points[2], .normal = norm, .tangent = tangents[1], .uvs = { uv[2] }, .color = Color::WHITE },
			Vertex{.location = points[0], .normal = norm, .tangent = tangents[1], .uvs = { uv[0] }, .color = Color::WHITE },
		};

		TList<SubMesh*> subMeshes;
		subMeshes.Add(new SubMesh{ vertices, {} });

		Mesh* mesh = new Mesh;
		mesh->m_submeshes = subMeshes;

		return mesh;
	}

	Mesh::Mesh() = default;

	Mesh::~Mesh()
	{
		for (SubMesh*& subMesh : m_submeshes)
		{
			subMesh->CleanOglObjects();
			delete subMesh;
		}
	}

	void Mesh::Render()
	{
		for (const SubMesh* subMesh : m_submeshes)
		{
			glBindVertexArray(subMesh->m_vao);
			if (subMesh->m_hasIndices)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh->m_ibo);
				glDrawElements(GL_TRIANGLES, static_cast<int32>(subMesh->m_triangleCount), GL_UNSIGNED_INT, nullptr);
			}
			else
			{
				glDrawArrays(GL_TRIANGLES, 0, static_cast<int32>(subMesh->m_vertexCount));
			}
		}
	}
}
