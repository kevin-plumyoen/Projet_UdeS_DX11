#include "sommetbloc.h"

#include <d3d11.h>
#include <DirectXMath.h>

namespace PM3D
{
  // Definir l'organisation de notre sommet
  D3D11_INPUT_ELEMENT_DESC CSommetBloc::layout[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,  0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };

  const uint32_t CSommetBloc::numElements = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

  CSommetBloc::CSommetBloc(const Vec3& position, const Vec3& normal, const Vec2& coordTex, const Vec3& tangent, const Vec3& bitangent)
    : m_Position(position)
    , m_Normal(normal)
    , m_CoordTex(coordTex)
    , m_Tangent(tangent)
    , m_Bitangent(bitangent)
  {
  }

} // namespace PM3D
