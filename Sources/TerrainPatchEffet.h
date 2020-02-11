#pragma once
#include "Objet3D.h"
#include "HitBox.h"

#include <terrain_patch.h>
#include <vector>

struct ID3D11Buffer;

namespace PM3D
{

    class CDispositifD3D11;

    class CTerrainPatchEffet : public CObjet3D
    {
    public:
        CTerrainPatchEffet(const ng::TerrainPatch& terrain, CDispositifD3D11* pDispositif);

        // Copy is disabled
        CTerrainPatchEffet(const CTerrainPatchEffet&) = delete;
        CTerrainPatchEffet& operator=(const CTerrainPatchEffet&) = delete;
        CTerrainPatchEffet(CTerrainPatchEffet&&);
        CTerrainPatchEffet& operator=(CTerrainPatchEffet&&);
        virtual ~CTerrainPatchEffet();
        void Draw() override;
        HitBox BoundingBox() const noexcept override;
    private:
        uint32_t nb_indices;

        CDispositifD3D11* pDispositif;

        ID3D11Buffer* pVertexBuffer;
        ID3D11Buffer* pIndexBuffer;
        HitBox bounding_box_;
    };
}
