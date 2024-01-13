#include "geometry.h"

#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>

#include <stdio.h>
#include <stdlib.h>

#define INDEX_SIZE sizeof(uint32_t)

struct Geometry* make_geometry(const char* filename, enum GeometryType type)
{
  struct Geometry* geometry = malloc(sizeof(struct Geometry));

  const struct aiScene* scene = aiImportFile(filename, type == GEOMETRY_TYPE_TRIS ? aiProcess_Triangulate : 0);
  if (!scene)
  {
    printf("Failed to load geometry \"%s\"\n", filename);
    free(geometry);
    return NULL;
  }

  // TODO: Properly support multiple meshes
  struct aiMesh* mesh = scene->mMeshes[0];

  uint32_t vertex_count = mesh->mNumVertices;
  geometry->index_count = mesh->mNumFaces * type;

  // Determine the vertex definition (ie. how many floats are required per vertex)
  {
    geometry->floats_per_vertex = 3; // Position

    if (mesh->mNormals)
    {
      geometry->floats_per_vertex += 3; // Normal
    }

    if (mesh->mTextureCoords[0])
    {
      geometry->floats_per_vertex += 2; // UV
    }
  }

  const uint32_t vertex_size = geometry->floats_per_vertex * sizeof(float);

  geometry->vertices = malloc(vertex_size * vertex_count);
  if (!geometry->vertices)
  {
    printf("Ran out of memory while loading geometry \"%s\", requested %u bytes for vertices\n", filename,
           vertex_size * vertex_count);
    aiReleaseImport(scene);
    free(geometry);
    return NULL;
  }

  geometry->indices = malloc(INDEX_SIZE * geometry->index_count);
  if (!geometry->indices)
  {
    printf("Ran out of memory while loading geometry \"%s\", requested %zu bytes for indices\n", filename,
           INDEX_SIZE * geometry->index_count);
    aiReleaseImport(scene);
    free(geometry);
    return NULL;
  }

  uint32_t vertex_counter = 0;
  for (unsigned int vertex_index = 0; vertex_index < mesh->mNumVertices; ++vertex_index)
  {
    uint32_t index = 0;

    const struct aiVector3D* vertex = &mesh->mVertices[vertex_index];
    geometry->vertices[vertex_counter * geometry->floats_per_vertex + index + 0] = vertex->x;
    geometry->vertices[vertex_counter * geometry->floats_per_vertex + index + 1] = vertex->y;
    geometry->vertices[vertex_counter * geometry->floats_per_vertex + index + 2] = vertex->z;
    index += 3;

    if (mesh->mNormals)
    {
      const struct aiVector3D* normal = &mesh->mNormals[vertex_index];
      geometry->vertices[vertex_counter * geometry->floats_per_vertex + index + 0] = normal->x;
      geometry->vertices[vertex_counter * geometry->floats_per_vertex + index + 1] = normal->y;
      geometry->vertices[vertex_counter * geometry->floats_per_vertex + index + 2] = normal->z;
      index += 3;
    }

    if (mesh->mTextureCoords[0])
    {
      const struct aiVector3D* uvs = &mesh->mTextureCoords[0][vertex_index];
      geometry->vertices[vertex_counter * geometry->floats_per_vertex + index + 0] = uvs->x;
      geometry->vertices[vertex_counter * geometry->floats_per_vertex + index + 1] = uvs->y;
    }

    ++vertex_counter;
  }

  for (unsigned int face_index = 0; face_index < mesh->mNumFaces; ++face_index)
  {
    const struct aiFace* face = &mesh->mFaces[face_index];

    if (face->mNumIndices != type)
    {
      printf("Geometry \"%s\" has invalid face with %d indices, expected %d\n", filename, face->mNumIndices, type);
      aiReleaseImport(scene);
      free(geometry);
      return NULL;
    }

    for (unsigned int index = 0; index < type; ++index)
    {
      geometry->indices[face_index * type + index] = face->mIndices[index];
    }
  }

  aiReleaseImport(scene);

  // Generate vertex array
  {
    glGenVertexArrays(1, &geometry->vertex_array);
    glBindVertexArray(geometry->vertex_array);

    // Generate and fill a vertex buffer
    {
      glGenBuffers(1, &geometry->vertex_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, geometry->vertex_buffer);
      glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertex_size * vertex_count), geometry->vertices, GL_STATIC_DRAW);
    }

    // Generate and fill an index buffer
    {
      glGenBuffers(1, &geometry->index_buffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->index_buffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(INDEX_SIZE * geometry->index_count), geometry->indices,
                   GL_STATIC_DRAW);
    }

    // Apply the vertex definition
    {
      // Position
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);

      // Normal
      if (mesh->mNormals)
      {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 3));
      }

      // UV
      if (mesh->mTextureCoords[0])
      {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)(sizeof(float) * 6));
      }
    }
  }

  return geometry;
}

void destroy_geometry(struct Geometry* geometry)
{
  free(geometry->vertices);
  free(geometry->indices);

  glDeleteBuffers(1, &geometry->index_buffer);
  glDeleteBuffers(1, &geometry->vertex_buffer);
  glDeleteVertexArrays(1, &geometry->vertex_array);

  free(geometry);
}
