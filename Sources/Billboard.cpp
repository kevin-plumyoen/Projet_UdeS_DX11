#include "Billboard.h"
#include "dispositifD3D11.h"
#include "sommetbloc.h"
#include "util.h"
#include "resource.h"

#include <vector>

Billboard::Billboard(Vec2 half_extend, PM3D::CDispositifD3D11* pDispositif)
: dispositif{pDispositif}
, vertices_buffer{nullptr} {
    std::vector<PM3D::CSommetBloc> sommets;
    sommets.reserve(6);

    // TODO: Calculate tangent and bitangent
    sommets.emplace_back(Vec3{0.f, -half_extend.x, -half_extend.y}, Vec3{0.f, 1.f, 0.f}, Vec2{0.f, 1.f});
    sommets.emplace_back(Vec3{0.f,  half_extend.x, -half_extend.y}, Vec3{0.f, 1.f, 0.f}, Vec2{1.f, 1.f});
    sommets.emplace_back(Vec3{0.f, -half_extend.x,  half_extend.y}, Vec3{0.f, 1.f, 0.f}, Vec2{0.f, 0.f});

    sommets.emplace_back(Vec3{0.f, -half_extend.x,  half_extend.y}, Vec3{0.f, 1.f, 0.f}, Vec2{0.f, 0.f});
    sommets.emplace_back(Vec3{0.f,  half_extend.x, -half_extend.y}, Vec3{0.f, 1.f, 0.f}, Vec2{1.f, 1.f});
    sommets.emplace_back(Vec3{0.f,  half_extend.x,  half_extend.y}, Vec3{0.f, 1.f, 0.f}, Vec2{1.f, 0.f});

    // Create buffer
    ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = 6 * sizeof(PM3D::CSommetBloc);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init_data;
    ZeroMemory(&init_data, sizeof(init_data));
    init_data.pSysMem = sommets.data();

    PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, &init_data, &vertices_buffer), DXE_CREATIONVERTEXBUFFER);

    hitbox_ = PM3D::HitBox(Vec3{0.f, 0.f, 0.f}, Vec3{1.f, half_extend.x, half_extend.y});
}

Billboard::~Billboard() {
    PM3D::DXRelacher(vertices_buffer);
}

void Billboard::Draw() {
    ID3D11DeviceContext* pImmediateContext = dispositif->GetImmediateContext();

    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const UINT stride = sizeof(PM3D::CSommetBloc);
    const UINT offset = 0;
    pImmediateContext->IASetVertexBuffers(0, 1, &vertices_buffer, &stride, &offset);

    pImmediateContext->Draw(6, 0);
}

PM3D::HitBox Billboard::BoundingBox() const noexcept {
    return hitbox_;
}