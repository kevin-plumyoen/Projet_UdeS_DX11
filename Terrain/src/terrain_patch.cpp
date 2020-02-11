#include "terrain_patch.h"

#include <cassert>
#include <iterator>
#include <algorithm>

namespace ng {
    TerrainPatch::TerrainPatch(dimension_type width, dimension_type height, float tile_size, std::vector<vertex_type>&& vertices, std::vector<normal_type>&& normals, std::vector<index_type>&& indices)
    : width_{ width }
    , height_{ height }
    , tile_size_{ tile_size }
    , vertices{ std::move(vertices) }
    , normals{ std::move(normals) }
    , indices{ std::move(indices) } {

    }

    std::vector<TerrainPatch::vertex_type>::size_type TerrainPatch::index_of_coordinate(dimension_type x, dimension_type y) const {
        if(x >= width_ || y >= height_) throw std::out_of_range{"terrain coordinate is out of range"};

        return y * width_ + x;
    }

    const TerrainPatch::vertex_type& TerrainPatch::vertex_at(dimension_type x, dimension_type y) const {
        return vertices[index_of_coordinate(x, y)];
    }
    
    TerrainPatch::vertex_type& TerrainPatch::vertex_at(dimension_type x, dimension_type y) {
        return vertices[index_of_coordinate(x, y)];
    }

    const TerrainPatch::normal_type& TerrainPatch::normal_at(dimension_type x, dimension_type y) const {
        return normals[index_of_coordinate(x, y)];
    }
    
    TerrainPatch::normal_type& TerrainPatch::normal_at(dimension_type x, dimension_type y) {
        return normals[index_of_coordinate(x, y)];
    }

    static const int MAGIC_SIZE = 4;
    static const char MAGIC_BYTES[MAGIC_SIZE] = {'T', 'P', 'A', 'T'};

    static const uint8_t CURRENT_VERSION = 1;

    std::ostream& TerrainPatch::save(std::ostream& stream) const noexcept {
        // Write a magic value at the start
        stream.write(MAGIC_BYTES, MAGIC_SIZE);
        stream.write(reinterpret_cast<const char*>(&CURRENT_VERSION), sizeof(CURRENT_VERSION));
        stream.write(reinterpret_cast<const char*>(&width_), sizeof(width_));
        stream.write(reinterpret_cast<const char*>(&height_), sizeof(height_));
        stream.write(reinterpret_cast<const char*>(&tile_size_), sizeof(tile_size_));

        // TODO: Compress this
        std::for_each(std::begin(vertices), std::end(vertices), [&stream](const TerrainPatch::vertex_type& vertex) {
            stream.write(reinterpret_cast<const char*>(&vertex), sizeof(vertex));
        });

        std::for_each(std::begin(normals), std::end(normals), [&stream](const TerrainPatch::normal_type& normal) {
            stream.write(reinterpret_cast<const char*>(&normal), sizeof(normal));
        });

        const uint32_t nb_faces = static_cast<uint32_t>(indices.size());
        stream.write(reinterpret_cast<const char*>(&nb_faces), sizeof(nb_faces));

        std::for_each(std::begin(indices), std::end(indices), [&stream](uint16_t index) {
            stream.write(reinterpret_cast<const char*>(&index), sizeof(index));
        });

        return stream;
    }

    std::istream& TerrainPatch::load(std::istream& stream) noexcept {
        vertices.clear();
        normals.clear();
        indices.clear();

        // Read and validate a magic value
        char magic[MAGIC_SIZE] = {0};
        stream.read(reinterpret_cast<char*>(magic), MAGIC_SIZE);
        for (int i = 0; i < MAGIC_SIZE; ++i) {
            if (magic[i] != MAGIC_BYTES[i]) {
                stream.setstate(std::ios::failbit);
                return stream;
            }
        }

        // Read and validate version
        uint8_t version;
        stream.read(reinterpret_cast<char*>(&version), sizeof(CURRENT_VERSION));
        if (version != CURRENT_VERSION) {
            stream.setstate(std::ios::failbit);
            return stream;
        }

        stream.read(reinterpret_cast<char*>(&width_), sizeof(width_));
        stream.read(reinterpret_cast<char*>(&height_), sizeof(height_));
        stream.read(reinterpret_cast<char*>(&tile_size_), sizeof(tile_size_));

        const uint32_t size = width_ * height_;
        vertices.reserve(size);
        normals.reserve(size);

        for (uint32_t i = 0; i < size; ++i) {
            TerrainPatch::vertex_type vertex;
            stream.read(reinterpret_cast<char*>(&vertex), sizeof(TerrainPatch::vertex_type));

            vertices.push_back(vertex);
        }
        
        for (uint32_t i = 0; i < size; ++i) {
            TerrainPatch::normal_type normal;
            stream.read(reinterpret_cast<char*>(&normal), sizeof(TerrainPatch::normal_type));

            normals.push_back(normal);
        }

        uint32_t nb_faces;
        stream.read(reinterpret_cast<char*>(&nb_faces), sizeof(nb_faces));
        indices.reserve(nb_faces);
        for (uint32_t i = 0; i < nb_faces; ++i) {
            TerrainPatch::index_type index;
            stream.read(reinterpret_cast<char*>(&index), sizeof(TerrainPatch::index_type));

            indices.push_back(index);
        }

        return stream;
    }


    TerrainPatchBuilder::TerrainPatchBuilder(dimension_type width, dimension_type height, float tile_size)
    : width_{width}
    , height_{height}
    , tile_size{tile_size} {
        vertices.reserve(width_ * height_);
        normals.reserve(width_ * height_);
    }

    std::vector<TerrainPatchBuilder::vertex_type>::size_type TerrainPatchBuilder::index_of_coordinate(dimension_type x, dimension_type y) const {
        if(x >= width_ || y >= height_) throw std::out_of_range{"terrain coordinate is out of range"};

        return y * width_ + x;
    }

    std::array<TerrainPatchBuilder::index_type, 6> TerrainPatchBuilder::quad_face_indices(index_type x, index_type y) const {
        assert(width_ <= std::numeric_limits<index_type>::max());
        const index_type w = static_cast<index_type>(width_);
        
        std::array<index_type, 6> indices_;
        indices_[0] = y * w + x;
        indices_[1] = y * w + (x + 1);
        indices_[2] = (y + 1) * w + (x + 1);

        indices_[3] = y * w + x;
        indices_[4] = (y + 1) * w + (x + 1);
        indices_[5] = (y + 1) * w + x;

        return indices_;
    }

    TerrainPatchBuilder::normal_type TerrainPatchBuilder::averaged_normal_at(dimension_type x, dimension_type y) const {
        normal_type n1, n2, n3, n4;
        vertex_type v1, v2, v3, v4;

        n1 = n2 = n3 = n4 = normal_type{0.f, 0.f, tile_size * tile_size};

        // Calculates vectors
        if (y < height_ - 1) {
            v1 = vertex_at(x, y + 1) - vertex_at(x, y);
        }

        if (x < width_ - 1) {
            v2 = vertex_at(x + 1, y) - vertex_at(x, y);
        }

        if (y > 0) {
            v3 = vertex_at(x, y - 1) - vertex_at(x, y);
        }

        if (x > 0) {
            v4 = vertex_at(x - 1, y) - vertex_at(x, y);
        }

        // Calculates normals
        if (y < height_ - 1 && x < width_ - 1) {
            n1 = v2.cross(v1);
        }
        else {
            n1 = {};
        }
        
        if (y > 0 && x < width_ - 1) {
            n2 = v3.cross(v2);
        }
        else {
            n2 = {};
        }

        if (y > 0 && x > 0) {
            n3 = v4.cross(v3);
        }
        else {
            n3 = {};
        }

        if (y < height_ - 1 && x > 0) {
            n4 = v1.cross(v4);
        }
        else {
            n4 = {};
        }

        return (n1 + n2 + n3 + n4).to_unit();
    }

    void TerrainPatchBuilder::calculate_normals() {
        for (std::size_t vertex_index = 0; vertex_index < vertices.size(); ++vertex_index) {
            const dimension_type x = static_cast<dimension_type>(vertex_index % width_);
            const dimension_type y = static_cast<dimension_type>(vertex_index / width_);

            normals[vertex_index] = averaged_normal_at(x, y);
        }
    }

    void TerrainPatchBuilder::calculate_indices() {
        indices.clear();

        const std::size_t nb_faces = width_ * height_ * 2;
        indices.reserve(nb_faces * 3);

        for (dimension_type y = 0; y < height_ - 1; ++y) {
            assert(y <= std::numeric_limits<index_type>::max());
            for (dimension_type x = 0; x < width_ - 1; ++x) {
                assert(x <= std::numeric_limits<index_type>::max());

                const std::array<index_type, 6> face_indices = quad_face_indices(static_cast<index_type>(x), static_cast<index_type>(y));
                std::copy(std::begin(face_indices), std::end(face_indices), std::back_inserter(indices));
            }
        }
    }

    void TerrainPatchBuilder::add_vertex(const vertex_type& vertex) {
        vertices.push_back(vertex);
        normals.push_back(Vec3f{0.f, 0.f, 1.f});
    }

    void TerrainPatchBuilder::add_vertex(vertex_type&& vertex) {
        vertices.push_back(std::move(vertex));
        normals.push_back(Vec3f{0.f, 0.f, 1.f});
    }

    const TerrainPatchBuilder::vertex_type& TerrainPatchBuilder::vertex_at(dimension_type x, dimension_type y) const {
        return vertices[index_of_coordinate(x, y)];
    }
    
    TerrainPatchBuilder::vertex_type& TerrainPatchBuilder::vertex_at(dimension_type x, dimension_type y) {
        return vertices[index_of_coordinate(x, y)];
    }

    const TerrainPatchBuilder::normal_type& TerrainPatchBuilder::normal_at(dimension_type x, dimension_type y) const {
        return normals[index_of_coordinate(x, y)];
    }
    
    TerrainPatchBuilder::normal_type& TerrainPatchBuilder::normal_at(dimension_type x, dimension_type y) {
        return normals[index_of_coordinate(x, y)];
    }

    TerrainPatch TerrainPatchBuilder::build() {
        const std::size_t total_size = width_ * height_;
        if(vertices.size() != total_size) throw IncompleteTerrainBuild{};
        if(normals.size() != total_size) throw IncompleteTerrainBuild{};

        calculate_normals();
        calculate_indices();

        return TerrainPatch{width_, height_, tile_size, std::move(vertices), std::move(normals), std::move(indices)};
    }

    std::vector<float> TerrainPatch::getHeights() const {
       std::vector<float> res = {};
       for (auto v : vertices) {
          res.push_back(v.z);
       }
       return res;
    }

    std::vector<float> TerrainPatch::getHeights(std::size_t row) const {
        std::vector<float> res = {};

        for (std::size_t i = 0; i < depth(); ++i) {
            res.push_back(vertex_at(i, row).z);
        }

        return res;
    }
}