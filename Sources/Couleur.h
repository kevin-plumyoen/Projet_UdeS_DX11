#ifndef COULEUR_H
#define COULEUR_H

#pragma warning(push, 0)
#include <iostream>
#include <DirectXMath.h>
#include <assimp/types.h>
#pragma warning(pop)

struct Couleur {
  using vector_type = DirectX::XMVECTOR;
  using value_type = float;
  using value_ref = value_type & ;

  vector_type values;

  Couleur() : values{ DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f) } {}
  Couleur(float r, float g, float b, float a) : values{ DirectX::XMVectorSet(r, g, b, a) } {}
  Couleur(const DirectX::XMVECTOR& vector) : values{ vector } {}
  Couleur(const Couleur&) = default;
  Couleur(const aiColor3D& c) : values{ DirectX::XMVectorSet(c.r, c.g, c.b, 1.0f) } {}

  operator DirectX::XMVECTOR() const { return values; };

  const value_type& r() const {
    return DirectX::XMVectorGetX(values);
  }
  const value_type& g() const {
    return DirectX::XMVectorGetY(values);
  }
  const value_type& b() const {
    return DirectX::XMVectorGetZ(values);
  }
  const value_type& a() const {
    return DirectX::XMVectorGetW(values);
  }

  void r(const value_type& r) {
    values = DirectX::XMVectorSetX(values, r);
  }
  void g(const value_type& g) {
    values = DirectX::XMVectorSetY(values, g);
  }
  void b(const value_type& b) {
    values = DirectX::XMVectorSetZ(values, b);
  }
  void a(const value_type& a) {
    values = DirectX::XMVectorSetW(values, a);
  }

};

#endif