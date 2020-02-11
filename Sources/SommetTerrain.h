#pragma once

#include "MathHelpers.h"

#include <d3d11.h>
#include <cstdint>

namespace PM3D
{

  class CSommetTerrain
  {
  public:
    CSommetTerrain() = default;
    CSommetTerrain(const Vec3& position, const Vec3& normal, const Vec2& coordTex = Vec2{ 0.0f, 0.0f }, const Vec2& coordLayout = Vec2{ 0.0f, 0.0f });

    static const uint32_t numElements;
    static D3D11_INPUT_ELEMENT_DESC layout[];

    const Vec3& position() const noexcept { return m_Position; }
    const Vec3& normal() const noexcept { return m_Normal; }
    const Vec2& uv() const noexcept { return m_CoordTex; }
    const Vec2& layout_uv() const noexcept { return m_CoordLayout; }
  private:
    Vec3 m_Position;
    Vec3 m_Normal;
    Vec2 m_CoordTex;
    Vec2 m_CoordLayout;
  };

} // namespace PM3D
