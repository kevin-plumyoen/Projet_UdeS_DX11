#ifndef SHADERTERRAIN_H
#define SHADERTERRAIN_H

#include "Shader.h"

class ShaderTerrain : public Shader {
  void passTexture(ID3D11ShaderResourceView* texture) = delete;
  void passNormal(ID3D11ShaderResourceView* texture) = delete;
public:

  ShaderTerrain(const wchar_t* path, PM3D::CDispositifD3D11* dispositif) : Shader(path) {
    init(dispositif);
  }

  void passTexture(ID3D11ShaderResourceView* texture,int n);
  void passNormal(ID3D11ShaderResourceView* texture, int n);
  void passEmission(ID3D11ShaderResourceView* texture, int n);
  void passLayout(ID3D11ShaderResourceView* texture);

  void init(PM3D::CDispositifD3D11*);
};

#endif
