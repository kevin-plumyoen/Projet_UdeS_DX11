#ifndef DEF_DYNAMIC_MESH_H
#define DEF_DYNAMIC_MESH_H

#include "objet3d.h"
#include "sommetbloc.h"
#include <vector>

namespace PM3D {
    class CDispositifD3D11;
}

struct Mesh;
struct ID3D11Buffer;

struct BufferTooSmallForMesh{};

class DynamicMesh : public PM3D::CObjet3D {
    const Mesh& mesh;
    PM3D::CDispositifD3D11* pDispositif;
    ID3D11Buffer* pVertexBuffer;
    ID3D11Buffer* pIndexBuffer;
    unsigned int index_count;
    PM3D::HitBox bounding_box_;

    std::vector<PM3D::CSommetBloc> vertices_from_mesh() const;
    void update_vertex_buffer();

public:
    DynamicMesh(const Mesh& m, PM3D::CDispositifD3D11* pDispositif);
    // Copy is disabled
    DynamicMesh(const DynamicMesh&) = delete;
    DynamicMesh& operator=(const DynamicMesh&) = delete;
    void Draw() override;
    PM3D::HitBox BoundingBox() const noexcept override;
};

#endif