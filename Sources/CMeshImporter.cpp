#include "CMeshImporter.h"
#include "sommetbloc.h"
#include "util.h"
#include "resource.h"
#include "dispositifD3D11.h"
#include "Mesh.h"

#include <cassert>
#include <algorithm>
#include <iterator>

#undef min
#undef max
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace PM3D;
using namespace DirectX;

CMeshImporter::CMeshImporter(const Mesh& mesh, CDispositifD3D11* pDispositif)
: pDispositif{pDispositif}
, pVertexBuffer(nullptr)
, pIndexBuffer(nullptr) {
    std::vector<CSommetBloc> sommets;
    sommets.reserve(mesh.vertices.size());

    if(mesh.has_tangent_and_bitangent()) {
        std::transform(mesh.vertices.begin(), mesh.vertices.end(), std::back_inserter(sommets), [](const Mesh::Vertex& vertex) {
            return CSommetBloc(vertex.position, vertex.normal, vertex.uv, vertex.tangent, vertex.bitangent);
        });
    }
    else {
        std::transform(mesh.vertices.begin(), mesh.vertices.end(), std::back_inserter(sommets), [](const Mesh::Vertex& vertex) {
            return CSommetBloc(vertex.position, vertex.normal, vertex.uv);
        });
    }

    bounding_box_ = HitBox::hitbox_from_mesh(mesh);

    nb_indices = static_cast<uint16_t>(mesh.indices.size());

    // Création du vertex buffer et copie des sommets
    ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = static_cast<UINT>(sommets.size() * sizeof(CSommetBloc));
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
    bd.ByteWidth = static_cast<UINT>(mesh.indices.size() * sizeof(uint16_t));
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = mesh.indices.data();
    pIndexBuffer = nullptr;

    DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer), DXE_CREATIONINDEXBUFFER);
}

CMeshImporter::~CMeshImporter() {
    DXRelacher(pVertexBuffer);
    DXRelacher(pIndexBuffer);
}

void CMeshImporter::Draw() {
    // Obtenir le contexte
    ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

    // Choisir la topologie des primitives
    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Source des sommets
    const UINT stride = sizeof(CSommetBloc);
    const UINT offset = 0;
    pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

    // Source des index
    pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    pImmediateContext->DrawIndexed(nb_indices, 0, 0);
}

HitBox CMeshImporter::BoundingBox() const noexcept {
    return bounding_box_;
}

