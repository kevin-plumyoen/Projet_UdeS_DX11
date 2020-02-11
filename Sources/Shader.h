#ifndef SHADER_H
#define SHADER_H

#include "DispositifD3D11.h"

#include "d3dx11effect.h"
#include <DirectXMath.h>

#define MAX_POINT_LIGHTS 100
#define MAX_DIR_LIGHTS 1
#define MAX_SPOT_LIGHTS 5

class Shader {

    struct ShadersParams {

    };

    struct InvalidBuffer {

    };

public:
    struct ShadersWorldParams {
        DirectX::XMMATRIX matWorldViewProj;	// la matrice totale 
        DirectX::XMMATRIX matWorld;			// matrice de transformation dans le monde

        DirectX::XMVECTOR vCamera; 			// la position de la caméra

        DirectX::XMVECTOR vAWorld;     // la valeur de la lumière ambiante
    };

    struct ShadersMaterialParams {
        DirectX::XMVECTOR vAMat; 			// la valeur ambiante du matériau
        DirectX::XMVECTOR vDMat; 			// la valeur diffuse du matériau
        DirectX::XMVECTOR vSMat; 			// la valeur specular du matériau 
        DirectX::XMVECTOR vEMat; 			// la valeur emissive du matériau

        float specForce;      // force de la specularite
        float specSpread;     // etendu de la specularite
        char padding[8];
    };

    struct ShadersLightCount
    {
        int PointLightsCount;	// nombre de point light
        int DirLightsCount;	// nombre de dir light  
        int SpotLightsCount;	// nombre de spot light  
        char padding[16 - sizeof(int) * 3]; // PADDING TO GET TO 16 BITS. DONT TOUCH !!!
    };

    struct ShadersPointLight {
        DirectX::XMVECTOR diffuseColor;  // Couleur diffuse de la lumière
        DirectX::XMVECTOR specularColor; // Couleur speculaire de la lumière
        DirectX::XMVECTOR pos;     // Position de la lumière
        float range;  // rayon de la lumière
        float intensity;  // intensité de la lumière
        char padding[16 - sizeof(float) * 2];
    };
    struct ShadersDirLight {
      DirectX::XMVECTOR diffuseColor;  // Couleur diffuse de la lumière
      DirectX::XMVECTOR specularColor; // Couleur speculaire de la lumière
      DirectX::XMVECTOR dir;     // Direction de la lumière
      float intensity;  // intensité de la lumière
      char padding[16 - sizeof(float)];
    };
    struct ShadersSpotLight {
      DirectX::XMVECTOR diffuseColor;  // Couleur diffuse de la lumière
      DirectX::XMVECTOR specularColor; // Couleur speculaire de la lumière
      DirectX::XMVECTOR pos;     // Position de la lumière
      DirectX::XMVECTOR dir;     // Direction de la lumière
      float range;  // rayon de la lumière
      float angle;
      float intensity;  // intensité de la lumière
      char padding[16 - sizeof(float)*3];
    };

    struct ShadersLightsParams {
        ShadersPointLight point_lights[MAX_POINT_LIGHTS]; // liste de point light
        ShadersDirLight dir_lights[MAX_DIR_LIGHTS]; // liste de point light
        ShadersSpotLight spot_lights[MAX_SPOT_LIGHTS]; // liste de point light
        //char padding[4];
    };

    struct ShadersTexturePresence {
        int diffuseSet; //use diffuse map
        int normalSet;  //use normal map
        int emissionSet;  //use normal map
        char padding[16 - sizeof(int) * 3]; // PADDING TO GET TO 16 BITS. DONT TOUCH !!!
    };

    const wchar_t* path;

    ID3DX11Effect* pEffet;
    ID3DX11EffectTechnique* pTechnique;
    ID3DX11EffectPass* pPasse;

    ID3D11InputLayout* pVertexLayout;

    ID3D11SamplerState* pSampleState;

    ID3D11Buffer* WorldParamsBuffer;
    ID3D11Buffer* MaterialParamsBuffer;
    ID3D11Buffer* TexturePresenceBuffer;
    ID3D11Buffer* LightsCountBuffer;
    ID3D11Buffer* LightsParamsBuffer;

    PM3D::CDispositifD3D11* pDispositif;
    
private:
    void init(PM3D::CDispositifD3D11*);
public:
    void apply();
    void passTexture(ID3D11ShaderResourceView* texture);
    void passNormal(ID3D11ShaderResourceView* texture);
    void passEmission(ID3D11ShaderResourceView* texture);
private:
    // Copy is disabled
    Shader & operator=(const Shader&) = delete;
    Shader(Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    template<typename T>
    struct is_buffer {
        static const bool value = false;
    };

    template<class T>
    struct trait_buffer;

    template<>
    struct trait_buffer<Shader::ShadersWorldParams> {
        static const char* name() { return "WORLD_PARAMS"; }
        static int id() { return 1; }
    };

    template<>
    struct is_buffer<Shader::ShadersWorldParams> {
        static const bool value = true;
    };

    template<>
    struct trait_buffer<Shader::ShadersMaterialParams> {
        static const char* name() { return "MATERIAL_PARAMS"; }
        static int id() { return 2; }
    };

    template<>
    struct is_buffer<Shader::ShadersMaterialParams> {
        static const bool value = true;
    };

    template<>
    struct trait_buffer<Shader::ShadersLightCount> {
        static const char* name() { return "LIGHTS_COUNT"; }
        static int id() { return 3; }
    };

    template<>
    struct is_buffer<Shader::ShadersLightCount> {
        static const bool value = true;
    };

    template<>
    struct trait_buffer<Shader::ShadersLightsParams> {
        static const char* name() { return "LIGHTS_PARAMS"; }
        static int id() { return 4; }
    };

    template<>
    struct is_buffer<Shader::ShadersLightsParams> {
        static const bool value = true;
    };

    template<>
    struct trait_buffer<Shader::ShadersTexturePresence> {
        static const char* name() { return "TEXTURE_PRESENCE"; }
        static int id() { return 5; }
    };

    template<>
    struct is_buffer<Shader::ShadersTexturePresence> {
        static const bool value = true;
    };

public:
    Shader(const wchar_t* path)
      : path{ path } {
    }
    Shader(const wchar_t* path, PM3D::CDispositifD3D11* dispositif) 
    : path{ path } {
        init(dispositif);
    }
    ~Shader();

    void swap(Shader& other) noexcept;

    template<class T>
    void fillBuffer(T sp) {
        static_assert(is_buffer<T>::value, "You must specify a valid buffer");
        ID3D11Buffer* b;
        switch (trait_buffer<T>::id()) {
        case 1:
            b = WorldParamsBuffer;
            break;
        case 2:
            b = MaterialParamsBuffer;
            break;
        case 3:
            b = LightsCountBuffer;
            break;
        case 4:
            b = LightsParamsBuffer;
            break;
        case 5:
            b = TexturePresenceBuffer;
            break;
        default: throw InvalidBuffer{};
        }

        ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
        pImmediateContext->UpdateSubresource(b, 0, nullptr, &sp, 0, 0);

        ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName(trait_buffer<T>::name());
        pCB->SetConstantBuffer(b);
    }

};

#endif
