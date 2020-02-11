#pragma once

#include "MathHelpers.h"

#include <d3d11.h>
#include <cstdint>

namespace PM3D
{

  class CSommetBloc
  {
  public:
    CSommetBloc() = default;
    CSommetBloc(const Vec3& position, const Vec3& normal, const Vec2& coordTex = Vec2{ 0.0f, 0.0f }, const Vec3& tangent = Vec3{ 0.0f,0.0f,0.0f }, const Vec3& bitangent = Vec3{0.0f, 0.0f, 0.0f});

        static const uint32_t numElements;
        static D3D11_INPUT_ELEMENT_DESC layout[];

        const Vec3& position() const noexcept { return m_Position; }
        const Vec3& normal() const noexcept { return m_Normal; }
        const Vec2& uv() const noexcept { return m_CoordTex; }
        const Vec3& tangent() const noexcept { return m_Tangent; }
        const Vec3& bitangent() const noexcept { return m_Bitangent; }
    private:
        Vec3 m_Position;
        Vec3 m_Normal;
        Vec2 m_CoordTex;
        Vec3 m_Tangent;
        Vec3 m_Bitangent;
    };

} // namespace PM3D