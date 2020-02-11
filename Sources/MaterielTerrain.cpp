#include "MaterielTerrain.h"
#include "Shader.h"
#include "dispositifD3D11.h"
#include "GestionnaireDeTextures.h"

MaterielTerrain::MaterielTerrain()
  : ambientColor(Couleur(1.0f, 1.0f, 1.0f, 1.0f))
  , diffuseColor(Couleur(1.0f, 1.0f, 1.0f, 1.0f))
  , specularColor(Couleur(1.0f, 1.0f, 1.0f, 1.0f))
  , emissionColor(Couleur(0.0f, 0.0f, 0.0f, 1.0f))
  , specularIntensity(0.5f)
  , specularSpread(8.0f)
  , diffuse1Map(nullptr)
  , diffuse2Map(nullptr)
  , diffuse3Map(nullptr)
  , normal1Map(nullptr)
  , normal2Map(nullptr)
  , normal3Map(nullptr)
  , emission1Map(nullptr)
  , emission2Map(nullptr)
  , emission3Map(nullptr)
  , layoutMap(nullptr)
  , shader(nullptr)
{}

MaterielTerrain::MaterielTerrain(Couleur a, Couleur d, Couleur s, Couleur e)
  : ambientColor(a)
  , diffuseColor(d)
  , specularColor(s)
  , emissionColor(e)
  , specularIntensity(0.5f)
  , specularSpread(8.0f)
  , diffuse1Map(nullptr)
  , diffuse2Map(nullptr)
  , diffuse3Map(nullptr)
  , normal1Map(nullptr)
  , normal2Map(nullptr)
  , normal3Map(nullptr)
  , emission1Map(nullptr)
  , emission2Map(nullptr)
  , emission3Map(nullptr)
  , layoutMap(nullptr)
  , shader(nullptr)
{}

MaterielTerrain::MaterielTerrain(MaterielTerrain&& other) noexcept
  : ambientColor(other.ambientColor)
  , diffuseColor(other.diffuseColor)
  , specularColor(other.specularColor)
  , emissionColor(other.emissionColor)
  , specularIntensity(other.specularIntensity)
  , specularSpread(other.specularSpread)
  , diffuse1Map(other.diffuse1Map)
  , diffuse2Map(other.diffuse2Map)
  , diffuse3Map(other.diffuse3Map)
  , normal1Map(other.normal1Map)
  , normal2Map(other.normal2Map)
  , normal3Map(other.normal3Map)
  , layoutMap(other.layoutMap)
  , shader(std::move(other.shader)) {
  other.diffuse1Map = nullptr;
  other.diffuse2Map = nullptr;
  other.diffuse3Map = nullptr;
  other.normal1Map = nullptr;
  other.normal2Map = nullptr;
  other.normal3Map = nullptr;
  other.emission1Map = nullptr;
  other.emission2Map = nullptr;
  other.emission3Map = nullptr;
  other.layoutMap = nullptr;
}

void MaterielTerrain::apply() const {
  if (!valid()) throw InvalidMateriel{};

  Shader::ShadersMaterialParams mp;
  mp.vAMat = ambientColor;
  mp.vDMat = diffuseColor;
  mp.vSMat = specularColor;
  mp.vEMat = emissionColor;
  mp.specForce = specularIntensity;
  mp.specSpread = specularSpread;
  shader->fillBuffer(mp);

  Shader::ShadersTexturePresence tp;
  if (layoutMap != nullptr) {
    tp.diffuseSet = (diffuse1Map != nullptr) && (diffuse2Map != nullptr) && (diffuse3Map != nullptr);
    tp.normalSet = (normal1Map != nullptr) && (normal2Map != nullptr) && (normal3Map != nullptr);
    tp.emissionSet = (emission1Map != nullptr) && (emission2Map != nullptr) && (emission3Map != nullptr);
  }
  else {
    tp.diffuseSet = false;
    tp.normalSet = false;
    tp.emissionSet = false;
  }
  shader->fillBuffer(tp);

  if (tp.diffuseSet) {
    shader->passTexture(diffuse1Map,1);
    shader->passTexture(diffuse2Map,2);
    shader->passTexture(diffuse3Map,3);
  }
  if (tp.normalSet) {
    shader->passNormal(normal1Map, 1);
    shader->passNormal(normal2Map, 2);
    shader->passNormal(normal3Map, 3);
  }
  if (tp.emissionSet) {
    shader->passEmission(emission1Map, 1);
    shader->passEmission(emission2Map, 2);
    shader->passEmission(emission3Map, 3);
  }
  if (tp.diffuseSet || tp.normalSet || tp.emissionSet)
    shader->passLayout(layoutMap);

  shader->apply();
}

bool MaterielTerrain::valid() const noexcept {
  return shader != nullptr;
}
