#include "Mesh.h"

#include <algorithm>

#undef min
#undef max
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static Vec3 to_vec3(const aiVector3D& v) noexcept {
    return Vec3{v.x, v.y, v.z};
}

static Vec2 to_vec2(const aiVector3D& v) noexcept {
    return Vec2{v.x, v.y};
}

Mesh::Mesh(const aiMesh* mesh) 
: Mesh{} {
    if (mesh->HasPositions() && mesh->HasNormals()) {
        has_tangent = mesh->HasTangentsAndBitangents();

        const bool use_uv = mesh->HasTextureCoords(0);

        if (has_tangent) {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                vertices.emplace_back(to_vec3(mesh->mVertices[i]), 
                                      to_vec3(mesh->mNormals[i]), 
                                      use_uv ? to_vec2( mesh->mTextureCoords[0][i]) : Vec2{0.f, 0.f},
                                      to_vec3(mesh->mTangents[i]),
                                      to_vec3(mesh->mBitangents[i]));
            }
        }
        else {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                vertices.emplace_back(to_vec3(mesh->mVertices[i]), 
                                      to_vec3(mesh->mNormals[i]), 
                                      use_uv ? to_vec2( mesh->mTextureCoords[0][i]) : Vec2{0.f, 0.f});
            }
        }

        if (mesh->HasFaces()) {
            for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
                const aiFace& face = mesh->mFaces[i];
                assert(face.mNumIndices == 3);
                for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                    assert(face.mIndices[j] <= std::numeric_limits<uint16_t>::max());
                    indices.push_back(static_cast<uint16_t>(face.mIndices[2 - j]));
                }
            }
        }
    }
    else {
        throw InvalidMesh{};
    }
}

bool Mesh::has_tangent_and_bitangent() const noexcept {
    return has_tangent;
}

bool Mesh::has_uv_coords() const noexcept {
    return has_uv;
}

