#ifndef DEF_MESH_H
#define DEF_MESH_H

#include "MathHelpers.h"

#include <vector>

struct aiMesh;

struct InvalidMesh{};

struct Mesh {
    struct Vertex {
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
        Vec3 tangent;
        Vec3 bitangent;

        Vertex() noexcept
        : position{ 0.f, 0.f, 0.f }
        , normal{ 0.f, 0.f, 0.f }
        , uv{ 0.f, 0.f } {

        }

        Vertex(const Vec3& position, const Vec3& normal, const Vec2& uv) noexcept
        : Vertex(position, normal, uv, Vec3{}, Vec3{}) {

        }

        Vertex(const Vec3& position, const Vec3& normal, const Vec2& uv, const Vec3& tangent, const Vec3& bitangent) noexcept
        : position{ position }
        , normal{ normal }
        , uv{ uv }
        , tangent{ tangent }
        , bitangent{ bitangent } {

        }
    };

    std::vector<Vertex> vertices = {};
    std::vector<uint16_t> indices = {};
    bool has_tangent = false;
    bool has_uv = false;

    Mesh() = default;
    explicit Mesh(const aiMesh* mesh);

    bool has_tangent_and_bitangent() const noexcept;
    bool has_uv_coords() const noexcept;
};

#endif