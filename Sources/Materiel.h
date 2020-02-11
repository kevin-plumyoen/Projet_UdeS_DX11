#ifndef MATERIEL_H
#define MATERIEL_H

#include "Couleur.h"
#include "Texture.h"
#include "Shader.h"

#include <string>
#include <memory>

namespace PM3D {
    class CDispositifD3D11;
    class CGestionnaireDeTextures;
}

struct aiMaterial;
struct aiString;

struct InvalidMateriel {};

class Materiel {
    std::wstring aiStringToWString(const aiString& aistr) const;
public:
    bool is_transparent;
    bool is_visible;
    Couleur ambientColor;
    Couleur diffuseColor;
    Couleur specularColor;
    Couleur emissionColor;

    float specularIntensity;
    float specularSpread;

    ID3D11ShaderResourceView* diffuseMap;
    ID3D11ShaderResourceView* normalMap;
    ID3D11ShaderResourceView* emissionMap;

    std::shared_ptr<Shader> shader;

    Materiel();
    Materiel(const Materiel&) = default;
    Materiel& operator=(const Materiel&) = default;
    Materiel(Materiel&& other) noexcept;
    Materiel& operator=(Materiel&& other) noexcept = default;
    Materiel(Couleur a, Couleur d, Couleur s, Couleur e);
    Materiel(const aiMaterial& m, PM3D::CGestionnaireDeTextures& g, PM3D::CDispositifD3D11* pDispositif);

    void load(const aiMaterial& m, PM3D::CGestionnaireDeTextures& g, PM3D::CDispositifD3D11* pDispositif);
    void apply() const;

    bool valid() const noexcept;

    template<typename ParamsType>
    void load_params(const ParamsType& p) {
        if(!valid()) throw InvalidMateriel{};

        shader->fillBuffer(p);
    }
};

#endif
