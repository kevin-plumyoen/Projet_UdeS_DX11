#include "Materiel.h"
#include "Shader.h"
#include "dispositifD3D11.h"
#include "GestionnaireDeTextures.h"

#undef min
#undef max
#include <assimp/material.h>

Materiel::Materiel()
: is_transparent(false)
, is_visible{true}
, ambientColor(Couleur(1.0f, 1.0f, 1.0f, 1.0f))
, diffuseColor(Couleur(1.0f, 1.0f, 1.0f, 1.0f))
, specularColor(Couleur(1.0f, 1.0f, 1.0f, 1.0f))
, emissionColor(Couleur(0.0f, 0.0f, 0.0f, 1.0f))
, specularIntensity(0.5f)
, specularSpread(8.0f)
, diffuseMap(nullptr)
, normalMap(nullptr)
, emissionMap(nullptr)
, shader(nullptr) 
{}

Materiel::Materiel(Couleur a, Couleur d, Couleur s, Couleur e)
: is_transparent(false)
, is_visible{true}
, ambientColor(a)
, diffuseColor(d)
, specularColor(s)
, emissionColor(e)
, specularIntensity(0.5f)
, specularSpread(8.0f)
, diffuseMap(nullptr)
, normalMap(nullptr)
, emissionMap(nullptr)
, shader(nullptr)
{}

Materiel::Materiel(Materiel&& other) noexcept
: is_transparent(other.is_transparent)
, is_visible{other.is_visible}
, ambientColor(other.ambientColor)
, diffuseColor(other.diffuseColor)
, specularColor(other.specularColor)
, emissionColor(other.emissionColor)
, specularIntensity(other.specularIntensity)
, specularSpread(other.specularSpread)
, diffuseMap(other.diffuseMap)
, normalMap(other.normalMap)
, shader(std::move(other.shader)) {
    other.diffuseMap = nullptr;
    other.normalMap = nullptr;
    other.emissionMap = nullptr;
}

Materiel::Materiel(const aiMaterial& m, PM3D::CGestionnaireDeTextures& g, PM3D::CDispositifD3D11* pDispositif)
: Materiel() {
    load(m, g, pDispositif);
}

std::wstring Materiel::aiStringToWString(const aiString& aistr) const {
    std::wstring temp_wstring;

    assert(aistr.length <= std::numeric_limits<int>::max());
    
    const int required_space = MultiByteToWideChar(CP_UTF8, 0, aistr.data, static_cast<int>(aistr.length), nullptr, 0);
    temp_wstring.resize(required_space);
    const int written_size = MultiByteToWideChar(CP_UTF8, 0, aistr.data, static_cast<int>(aistr.length), temp_wstring.data(), required_space);
    
    if (written_size == 0) throw std::runtime_error("failed to convert input string");

    return temp_wstring;
}

void Materiel::load(const aiMaterial& m, PM3D::CGestionnaireDeTextures& g, PM3D::CDispositifD3D11* pDispositif) {
    //if(!valid()) throw InvalidMateriel{};
    
    aiColor3D color;
    m.Get(AI_MATKEY_COLOR_AMBIENT, color);
    ambientColor = Couleur(color);
    m.Get(AI_MATKEY_COLOR_DIFFUSE, color);
    diffuseColor = Couleur(color);
    m.Get(AI_MATKEY_COLOR_SPECULAR, color);
    specularColor = Couleur(color);
    m.Get(AI_MATKEY_COLOR_EMISSIVE, color);
    emissionColor = Couleur(color);

    m.Get(AI_MATKEY_SHININESS, specularSpread);
    m.Get(AI_MATKEY_SHININESS_STRENGTH, specularIntensity);

    aiString temp_ai_string;

    if (m.GetTextureCount(aiTextureType_DIFFUSE)) {
        m.GetTexture(aiTextureType_DIFFUSE, 0, &temp_ai_string);
        diffuseMap = g.GetNewTexture(aiStringToWString(temp_ai_string), pDispositif)->GetD3DTexture();
    }
    if (m.GetTextureCount(aiTextureType_NORMALS)) {
        m.GetTexture(aiTextureType_NORMALS, 0, &temp_ai_string);
        normalMap = g.GetNewTexture(aiStringToWString(temp_ai_string), pDispositif)->GetD3DTexture();
    }
    if (m.GetTextureCount(aiTextureType_EMISSIVE)) {
        m.GetTexture(aiTextureType_EMISSIVE, 0, &temp_ai_string);
        emissionMap = g.GetNewTexture(aiStringToWString(temp_ai_string), pDispositif)->GetD3DTexture();
    }
}

void Materiel::apply() const {
    if(!valid()) throw InvalidMateriel{};

    Shader::ShadersMaterialParams mp;
    mp.vAMat = ambientColor;
    mp.vDMat = diffuseColor;
    mp.vSMat = specularColor;
    mp.vEMat = emissionColor;
    mp.specForce = specularIntensity;
    mp.specSpread = specularSpread;
    shader->fillBuffer(mp);

    Shader::ShadersTexturePresence tp;
    tp.diffuseSet = (diffuseMap != nullptr);
    tp.normalSet = (normalMap != nullptr);
    tp.emissionSet = (emissionMap != nullptr);
    shader->fillBuffer(tp);

    if (tp.diffuseSet)
        shader->passTexture(diffuseMap);
    if (tp.normalSet)
        shader->passNormal(normalMap);
    if (tp.emissionSet)
        shader->passEmission(emissionMap);

    shader->apply();
}

bool Materiel::valid() const noexcept {
    return shader != nullptr;
}
