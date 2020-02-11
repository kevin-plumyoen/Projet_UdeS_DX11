#include "TerrainPatchEffet.h"
#include "sommetTerrain.h"
#include "util.h"
#include "DispositifD3D11.h"
#include "resource.h"
#include "Camera.h"
#include "DrawContext.h"
#include "Shader.h"
#include "Mesh.h"

#include <d3dcompiler.h>
#undef max
#undef min

#include <algorithm>
#include <iterator>
#include <cmath>
#include <string>
#include <limits>
#include <cassert>

using namespace DirectX;

namespace PM3D {

    CTerrainPatchEffet::CTerrainPatchEffet(const ng::TerrainPatch& terrain, CDispositifD3D11* pDispositif)
    : pDispositif(pDispositif)
    , pVertexBuffer(nullptr)
    , pIndexBuffer(nullptr) {
        std::vector<CSommetTerrain> sommets;
        sommets.reserve(std::distance(terrain.begin_vertices(), terrain.end_vertices()));

        int numVertex = 0;

        std::transform(terrain.begin_vertices(), terrain.end_vertices(),
            terrain.begin_normals(), std::back_inserter(sommets),
            [&terrain, &numVertex](const ng::TerrainPatch::vertex_type& vertex, const ng::TerrainPatch::normal_type& normal) {
            XMFLOAT3 vert;
            vert.x = vertex.x;
            vert.y = vertex.y;
            vert.z = vertex.z;

            XMFLOAT3 norm;
            norm.x = normal.x;
            norm.y = normal.y;
            norm.z = normal.z;

            XMFLOAT2 uv;

            uv.x = static_cast<float>(numVertex % terrain.width());
            uv.y = static_cast<float>(numVertex / terrain.width());

            XMFLOAT2 layout;

            layout.x = uv.x / terrain.width();
            layout.y = uv.y / terrain.depth();

            ++numVertex;

            return CSommetTerrain(vert, norm, uv, layout);
        });

        std::vector<uint16_t> indices(terrain.begin_indices(), terrain.end_indices());
        assert(indices.size() < std::numeric_limits<uint32_t>::max());
        nb_indices = static_cast<uint32_t>(indices.size());

        Mesh m;
        m.vertices.reserve(sommets.size());
        std::transform(sommets.begin(), sommets.end(), 
                       std::back_inserter(m.vertices), 
                       [](const CSommetTerrain& vertex) {
            return Mesh::Vertex{vertex.position(), vertex.normal(), vertex.uv()};
        });
        bounding_box_ = HitBox::hitbox_from_mesh(m);

        // Création du vertex buffer et copie des sommets
        ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = static_cast<UINT>(sommets.size() * sizeof(CSommetTerrain));
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = sommets.data();
        pVertexBuffer = nullptr;

        DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

        // Création de l'index buffer et copie des indices
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint16_t));
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = indices.data();
        pIndexBuffer = nullptr;

        DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
            DXE_CREATIONINDEXBUFFER);
    }

    CTerrainPatchEffet::CTerrainPatchEffet(CTerrainPatchEffet&& other)
    : nb_indices{ other.nb_indices }
    , pDispositif{ other.pDispositif }
    , pVertexBuffer{ other.pVertexBuffer }
    , pIndexBuffer{ other.pIndexBuffer } {
        other.pIndexBuffer = nullptr;
        other.pVertexBuffer = nullptr;
    }

    CTerrainPatchEffet& CTerrainPatchEffet::operator=(CTerrainPatchEffet&& other) {
        using std::swap;
        swap(nb_indices, other.nb_indices);
        swap(pDispositif, other.pDispositif);
        swap(pVertexBuffer, other.pVertexBuffer);
        swap(pIndexBuffer, other.pIndexBuffer);

        return *this;
    }

    CTerrainPatchEffet::~CTerrainPatchEffet() {
        DXRelacher(pVertexBuffer);
        DXRelacher(pIndexBuffer);
    }

    void CTerrainPatchEffet::Draw() {
        // Obtenir le contexte
        ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

        // Choisir la topologie des primitives
        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Source des sommets
        const UINT stride = sizeof(CSommetTerrain);
        const UINT offset = 0;
        pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

        // Source des index
        pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        pImmediateContext->DrawIndexed(nb_indices, 0, 0);
    }

    HitBox CTerrainPatchEffet::BoundingBox() const noexcept {
        return bounding_box_;
    }
}