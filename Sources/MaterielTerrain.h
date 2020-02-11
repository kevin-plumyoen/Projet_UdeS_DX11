#ifndef MATERIELTERRAIN_H
#define MATERIELTERRAIN_H

#include "Couleur.h"
#include "Texture.h"
#include "Materiel.h"
#include "ShaderTerrain.h"

#include <string>
#include <memory>

namespace PM3D {
  class CDispositifD3D11;
  class CGestionnaireDeTextures;
}

class MaterielTerrain{
public:
  Couleur ambientColor;
  Couleur diffuseColor;
  Couleur specularColor;
  Couleur emissionColor;

  float specularIntensity;
  float specularSpread;

  ID3D11ShaderResourceView* diffuse1Map;
  ID3D11ShaderResourceView* diffuse2Map;
  ID3D11ShaderResourceView* diffuse3Map;
  ID3D11ShaderResourceView* normal1Map;
  ID3D11ShaderResourceView* normal2Map;
  ID3D11ShaderResourceView* normal3Map;
  ID3D11ShaderResourceView* emission1Map;
  ID3D11ShaderResourceView* emission2Map;
  ID3D11ShaderResourceView* emission3Map;
  ID3D11ShaderResourceView* layoutMap;

  std::shared_ptr<ShaderTerrain> shader;

  MaterielTerrain();
  MaterielTerrain(const MaterielTerrain&) = delete;
  MaterielTerrain& operator=(const MaterielTerrain&) = delete;
  MaterielTerrain(MaterielTerrain&& other) noexcept;
  MaterielTerrain& operator=(MaterielTerrain&& other) noexcept = default;
  MaterielTerrain(Couleur a, Couleur d, Couleur s, Couleur e);

  void apply() const;

  bool valid() const noexcept;

  template<typename ParamsType>
  void load_params(const ParamsType& p) {
    if (!valid()) throw InvalidMateriel{};

    shader->fillBuffer(p);
  }
};

#endif
