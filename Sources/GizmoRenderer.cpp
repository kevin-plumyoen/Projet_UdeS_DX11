#include "GizmoRenderer.h"
#include "util.h"
#include "dispositifD3D11.h"
#include "MathHelpers.h"

#include <vector>

struct GizmoVertex {
    Vec3 position;
    Color color;

    GizmoVertex(Vec3 position, Color color) noexcept
    : position{ position }
    , color{ color } {

    }
};

void GizmoRenderer::init_shape() {
    std::vector<GizmoVertex> vertices;
    vertices.reserve(24);

    // Z Axis
    vertices.emplace_back(Vec3{-0.1f, -0.1f, -0.1f}, Color{0.f, 0.f, 1.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f, -0.1f, -0.1f}, Color{0.f, 0.f, 1.f, 1.f});
    vertices.emplace_back(Vec3{-0.1f, -0.1f,  0.9f}, Color{0.f, 0.f, 1.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f, -0.1f,  0.9f}, Color{0.f, 0.f, 1.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f,  0.1f, -0.1f}, Color{0.f, 0.f, 1.f, 1.f});
    vertices.emplace_back(Vec3{-0.1f,  0.1f, -0.1f}, Color{0.f, 0.f, 1.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f,  0.1f,  0.9f}, Color{0.f, 0.f, 1.f, 1.f});
    vertices.emplace_back(Vec3{-0.1f,  0.1f,  0.9f}, Color{0.f, 0.f, 1.f, 1.f});

    // Y Axis
    vertices.emplace_back(Vec3{ 0.1f,  0.1f, -0.1f}, Color{0.f, 1.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{-0.1f,  0.1f, -0.1f}, Color{0.f, 1.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f,  0.9f, -0.1f}, Color{0.f, 1.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{-0.1f,  0.9f, -0.1f}, Color{0.f, 1.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{-0.1f,  0.9f,  0.1f}, Color{0.f, 1.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f,  0.9f,  0.1f}, Color{0.f, 1.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{-0.1f,  0.1f,  0.1f}, Color{0.f, 1.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f,  0.1f,  0.1f}, Color{0.f, 1.f, 0.f, 1.f});

    // X Axis
    vertices.emplace_back(Vec3{ 0.1f,  0.1f, -0.1f}, Color{1.f, 0.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f, -0.1f, -0.1f}, Color{1.f, 0.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f,  0.1f,  0.1f}, Color{1.f, 0.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.1f, -0.1f,  0.1f}, Color{1.f, 0.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.9f,  0.1f, -0.1f}, Color{1.f, 0.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.9f, -0.1f, -0.1f}, Color{1.f, 0.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.9f, -0.1f,  0.1f}, Color{1.f, 0.f, 0.f, 1.f});
    vertices.emplace_back(Vec3{ 0.9f,  0.1f,  0.1f}, Color{1.f, 0.f, 0.f, 1.f});

    const std::vector<uint16_t> indices = {
        // Z Axis
        1, 2, 3, 2, 4, 3,
        2, 5, 7, 2, 7, 4,
        5, 6, 8, 5, 8, 7,
        6, 1, 3, 6, 3, 8,
        8, 3, 4, 8, 4, 7,
        5, 2, 1, 5, 1, 6,

        // Y Axis
        8 + 3, 8 + 6, 8 + 8, 8 + 3, 8 + 8, 8 + 1,
        8 + 4, 8 + 5, 8 + 3, 8 + 3, 8 + 5, 8 + 6,
        8 + 5, 8 + 7, 8 + 6, 8 + 6, 8 + 7, 8 + 8,
        8 + 3, 8 + 2, 8 + 4, 8 + 3, 8 + 1, 8 + 2,
        8 + 4, 8 + 7, 8 + 5, 8 + 4, 8 + 2, 8 + 7,

        // X Axis
        16 + 1, 16 + 3, 16 + 5, 16 + 3, 16 + 8, 16 + 5,
        16 + 5, 16 + 8, 16 + 6, 16 + 6, 16 + 8, 16 + 7,
        16 + 3, 16 + 4, 16 + 8, 16 + 4, 16 + 7, 16 + 8,
        16 + 6, 16 + 4, 16 + 2, 16 + 6, 16 + 7, 16 + 4,
        16 + 5, 16 + 2, 16 + 1, 16 + 5, 16 + 6, 16 + 2
    };

    ID3D11Device* pD3DDevice = dispositif->GetD3DDevice();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(GizmoVertex));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices.data();

    PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer));

    // Création de l'index buffer et copie des indices
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint16_t));
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices.data();

    PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer));
}

GizmoRenderer::GizmoRenderer(PM3D::CDispositifD3D11* dispositif)
: dispositif{dispositif}
, pIndexBuffer{}
, pVertexBuffer{} {
    init_shape();
}

GizmoRenderer::~GizmoRenderer() {
    PM3D::DXRelacher(pVertexBuffer);
    PM3D::DXRelacher(pIndexBuffer);
}

void GizmoRenderer::render(ID3D11DeviceContext*, std::vector<Mat4x4>&&) {
    // Draw instanced
}