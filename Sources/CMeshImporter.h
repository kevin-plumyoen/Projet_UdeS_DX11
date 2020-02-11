#pragma once

#include "Objet3D.h"
#include "MathHelpers.h"
#include "SommetBloc.h"
#include "Mesh.h"

#include <vector>

struct ID3D11Buffer;

namespace PM3D {
    class CDispositifD3D11;

    class CMeshImporter : public CObjet3D {
    public:
        CMeshImporter(const Mesh& mesh, CDispositifD3D11* pDispositif);
        CMeshImporter(const CMeshImporter&) = delete;
        CMeshImporter& operator=(const CMeshImporter&) = delete;
        virtual ~CMeshImporter();
        void Draw() override;
        HitBox BoundingBox() const noexcept override;
    protected:
        CDispositifD3D11* pDispositif;
        ID3D11Buffer* pVertexBuffer;
        ID3D11Buffer* pIndexBuffer;

        uint16_t nb_indices;
        HitBox bounding_box_;
    };
}
