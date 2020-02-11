#ifndef DEF_HEIGHMAP_MESH_H
#define DEF_HEIGHMAP_MESH_H

#include "vector.h"
#include <vector>
#include <array>
#include <iostream>

namespace ng {

class TerrainPatchBuilder;

class TerrainPatch {
    friend TerrainPatchBuilder;
public:
    using index_type = uint16_t;
    using dimension_type = uint32_t;
    using vertex_type = Vec3f;
    using normal_type = Vec3f;
private:
    dimension_type width_ = {}, height_ = {};
    float tile_size_ = {1.f};
    std::vector<vertex_type> vertices;
    std::vector<normal_type> normals;
    std::vector<index_type> indices;

    std::vector<vertex_type>::size_type index_of_coordinate(dimension_type x, dimension_type y) const;

    TerrainPatch(dimension_type width, dimension_type height, float tile_size, std::vector<vertex_type>&& vertices, std::vector<normal_type>&& normals, std::vector<index_type>&& indices);
public:
    using normal_iterator = std::vector<normal_type>::const_iterator;
    using vertex_iterator = std::vector<vertex_type>::const_iterator;
    using index_iterator = std::vector<index_type>::const_iterator;

    TerrainPatch() noexcept = default;

    const vertex_type& vertex_at(dimension_type x, dimension_type y) const;
    vertex_type& vertex_at(dimension_type x, dimension_type y);

    const normal_type& normal_at(dimension_type x, dimension_type y) const;
    normal_type& normal_at(dimension_type x, dimension_type y);

    std::ostream& save(std::ostream& stream) const noexcept;
    std::istream& load(std::istream& stream) noexcept;

    normal_iterator begin_normals() const { return normals.begin(); }
    normal_iterator end_normals() const { return normals.end(); }

    vertex_iterator begin_vertices() const { return vertices.begin(); }
    vertex_iterator end_vertices() const { return vertices.end(); }

    index_iterator begin_indices() const { return indices.begin(); }
    index_iterator end_indices() const { return indices.end(); }

    dimension_type width() const noexcept { return width_; }
    dimension_type depth() const noexcept { return height_; }
    float tile_size() const noexcept { return tile_size_; }

    std::vector<float> getHeights() const;
    std::vector<float> getHeights(std::size_t row) const;

};

class IncompleteTerrainBuild{};

class TerrainPatchBuilder {
public:
    using index_type = TerrainPatch::index_type;
    using dimension_type = TerrainPatch::dimension_type;
    using vertex_type = TerrainPatch::vertex_type;
    using normal_type = TerrainPatch::normal_type;
private:
    dimension_type width_ = {}, height_ = {};
    float tile_size;
    std::vector<vertex_type> vertices;
    std::vector<normal_type> normals;
    std::vector<index_type> indices;

    std::vector<vertex_type>::size_type index_of_coordinate(dimension_type x, dimension_type y) const;
    std::array<index_type, 6> quad_face_indices(index_type x, index_type y) const;
    normal_type averaged_normal_at(dimension_type x, dimension_type y) const;

    void calculate_normals();
    void calculate_indices();

public:
    TerrainPatchBuilder(dimension_type width, dimension_type height, float tile_size = 1.f);

    void add_vertex(const vertex_type& vertex);
    void add_vertex(vertex_type&& vertex);

    const vertex_type& vertex_at(dimension_type x, dimension_type y) const;
    vertex_type& vertex_at(dimension_type x, dimension_type y);

    const normal_type& normal_at(dimension_type x, dimension_type y) const;
    normal_type& normal_at(dimension_type x, dimension_type y);

    TerrainPatch build();
};

}

#endif