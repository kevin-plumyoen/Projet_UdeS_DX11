#include "ScreenBillboard.h"
#include "util.h"
#include "dispositifD3D11.h"
#include "resource.h"

#include <d3d11.h>

CScreenBillboard::CScreenBillboard(Vec2 center, Vec2 half_extends, PM3D::CDispositifD3D11* pDispositif) 
: pDispositif{pDispositif} 
, pVertexBuffer{nullptr} 
, bounding_box_{Vec3{0.f, 0.f, 0.f}, Vec3{half_extends.x, half_extends.y, 0.f}} {
    const float min_x = center.x - half_extends.x;
    const float min_y = center.y - half_extends.y;
    const float max_x = center.x + half_extends.x;
    const float max_y = center.y + half_extends.y;

    // 0--------1
    // |        |
    // |        |
    // |        |
    // 2--------3

    CScreenBillboardVertex vertices[] = {
        // Triangle 0, 1, 3
        {Vec3{min_x, max_y, 0.f}, Vec2{0.f, 0.f}},
        {Vec3{max_x, max_y, 0.f}, Vec2{1.f, 0.f}},
        {Vec3{max_x, min_y, 0.f}, Vec2{1.f, 1.f}},

        // Triangle 3, 2, 0
        {Vec3{max_x, min_y, 0.f}, Vec2{1.f, 1.f}},
        {Vec3{min_x, min_y, 0.f}, Vec2{0.f, 1.f}},
        {Vec3{min_x, max_y, 0.f}, Vec2{0.f, 0.f}},
    };

    ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = static_cast<UINT>(6 * (sizeof(Vec3) + sizeof(Vec2)));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    pVertexBuffer = nullptr;

    PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
}

CScreenBillboard::~CScreenBillboard() {
    PM3D::DXRelacher(pVertexBuffer);
}

void CScreenBillboard::Draw() {
    ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

    // Choisir la topologie des primitives
    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Source des sommets
    const UINT stride = sizeof(Vec3) + sizeof(Vec2);
    const UINT offset = 0;
    pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

    pImmediateContext->Draw(6, 0);
}

PM3D::HitBox CScreenBillboard::BoundingBox() const noexcept {
    return bounding_box_;
}