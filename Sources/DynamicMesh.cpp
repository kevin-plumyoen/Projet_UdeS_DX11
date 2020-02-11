#include "DynamicMesh.h"
#include "Mesh.h"
#include "sommetbloc.h"
#include "dispositifD3D11.h"
#include "util.h"
#include "resource.h"

#include <vector>
#include <algorithm>
#include <cassert>

DynamicMesh::DynamicMesh(const Mesh& m, PM3D::CDispositifD3D11* pDispositif)
: mesh{m}
, pDispositif{pDispositif}
, pVertexBuffer{nullptr}
, pIndexBuffer{nullptr}
, index_count{static_cast<unsigned int>(mesh.indices.size())} {
    const std::vector<PM3D::CSommetBloc> vertices = vertices_from_mesh();

    ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(PM3D::CSommetBloc));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices.data();

    PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

    // Création de l'index buffer et copie des indices
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = static_cast<UINT>(mesh.indices.size() * sizeof(uint16_t));
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = mesh.indices.data();

    PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer), DXE_CREATIONINDEXBUFFER);
}

std::vector<PM3D::CSommetBloc> DynamicMesh::vertices_from_mesh() const {
    std::vector<PM3D::CSommetBloc> vertices;
    vertices.reserve(mesh.vertices.size());

    std::transform(mesh.vertices.begin(), mesh.vertices.end(), 
                   std::back_inserter(vertices), [](const Mesh::Vertex& vertex) {
        return PM3D::CSommetBloc(vertex.position, vertex.normal, vertex.uv);
    });

    return vertices;
}

void DynamicMesh::update_vertex_buffer() {
    if (mesh.indices.size() > index_count) {
        throw BufferTooSmallForMesh{};
    }

    bounding_box_ = PM3D::HitBox::hitbox_from_mesh(mesh);

    const std::vector<PM3D::CSommetBloc> vertices = vertices_from_mesh();

    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    ZeroMemory(&mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
    pImmediateContext->Map(pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

    std::copy(vertices.begin(), vertices.end(), 
              reinterpret_cast<PM3D::CSommetBloc*>(mapped_resource.pData));

    pImmediateContext->Unmap(pVertexBuffer, 0);
}

void DynamicMesh::Draw() {
    update_vertex_buffer();

    // Obtenir le contexte
    ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

    // Choisir la topologie des primitives
    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Source des sommets
    const UINT stride = sizeof(PM3D::CSommetBloc);
    const UINT offset = 0;
    pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

    // Source des index
    pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    pImmediateContext->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
}

PM3D::HitBox DynamicMesh::BoundingBox() const noexcept {
    return bounding_box_;
}