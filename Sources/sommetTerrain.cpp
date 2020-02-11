#include "sommetTerrain.h"

#include <d3d11.h>
#include <DirectXMath.h>

namespace PM3D
{
  // Definir l'organisation de notre sommet
  D3D11_INPUT_ELEMENT_DESC CSommetTerrain::layout[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,  0 },
  { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "LAYOUTCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };

  const uint32_t CSommetTerrain::numElements = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

  CSommetTerrain::CSommetTerrain(const Vec3& position, const Vec3& normal, const Vec2& coordTex, const Vec2& coordLayout)
    : m_Position(position)
    , m_Normal(normal)
    , m_CoordTex(coordTex)
    , m_CoordLayout(coordLayout)
  {
  }

} // namespace PM3D
