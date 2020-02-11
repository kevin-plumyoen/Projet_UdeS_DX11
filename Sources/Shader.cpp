#include "resource.h"
#include "Shader.h"
#include "util.h"
#include "SommetBloc.h"

#include <d3dcompiler.h>

#include <algorithm>

using namespace DirectX;
using namespace PM3D;

Shader::Shader(Shader&& other) noexcept
: path{other.path} 
, pEffet{other.pEffet}
, pTechnique{other.pTechnique} 
, pPasse{other.pPasse} 
, pVertexLayout{other.pVertexLayout} 
, pSampleState{other.pSampleState} 
, WorldParamsBuffer{other.WorldParamsBuffer}
, MaterialParamsBuffer{other.MaterialParamsBuffer}
, TexturePresenceBuffer{other.TexturePresenceBuffer} 
, LightsCountBuffer{other.LightsCountBuffer}
, LightsParamsBuffer{other.LightsParamsBuffer} {
    other.path = nullptr;
    other.pEffet = nullptr;
    other.pTechnique = nullptr;
    other.pPasse = nullptr;
    other.pVertexLayout = nullptr;
    other.pSampleState = nullptr;
    other.WorldParamsBuffer = nullptr;
    other.MaterialParamsBuffer = nullptr;
    other.TexturePresenceBuffer = nullptr;
    other.LightsCountBuffer = nullptr;
    other.LightsParamsBuffer = nullptr;
}

void Shader::swap(Shader& other) noexcept {
    using std::swap;

    swap(path, other.path);
    swap(pEffet, other.pEffet);
    swap(pTechnique, other.pTechnique);
    swap(pPasse, other.pPasse);
    swap(pVertexLayout, other.pVertexLayout);
    swap(pSampleState, other.pSampleState);
    swap(WorldParamsBuffer, other.WorldParamsBuffer);
    swap(MaterialParamsBuffer, other.MaterialParamsBuffer);
    swap(TexturePresenceBuffer, other.TexturePresenceBuffer);
    swap(LightsCountBuffer, other.LightsCountBuffer);
    swap(LightsParamsBuffer, other.LightsParamsBuffer);
}

Shader& Shader::operator=(Shader&& other) noexcept {
    swap(other);

    return *this;
}

void Shader::init(PM3D::CDispositifD3D11* Dispositif) {
  pDispositif = Dispositif;

  // Compilation et chargement du vertex shader
  ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

/////////////////////////// Création des buffer pour les paramètres du shader ////////////////////////////

  // World Params
  D3D11_BUFFER_DESC bd1;
  ZeroMemory(&bd1, sizeof(bd1));
  bd1.Usage = D3D11_USAGE_DEFAULT;
  bd1.ByteWidth = sizeof(ShadersWorldParams);
  bd1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd1.CPUAccessFlags = 0;
  DXEssayer(pD3DDevice->CreateBuffer(&bd1, nullptr, &WorldParamsBuffer));

    // Material Params
  D3D11_BUFFER_DESC bd2;
  ZeroMemory(&bd2, sizeof(bd2));
  bd2.Usage = D3D11_USAGE_DEFAULT;
  bd2.ByteWidth = sizeof(ShadersMaterialParams);
  bd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd2.CPUAccessFlags = 0;
  DXEssayer(pD3DDevice->CreateBuffer(&bd2, nullptr, &MaterialParamsBuffer));

    // Texture Presence
  D3D11_BUFFER_DESC bd3;
  ZeroMemory(&bd3, sizeof(bd3));
  bd3.Usage = D3D11_USAGE_DEFAULT;
  bd3.ByteWidth = sizeof(ShadersTexturePresence);
  bd3.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd3.CPUAccessFlags = 0;
  DXEssayer(pD3DDevice->CreateBuffer(&bd3, nullptr, &TexturePresenceBuffer));

    // Light Param
  D3D11_BUFFER_DESC bd4;
  ZeroMemory(&bd4, sizeof(bd4));
  bd4.Usage = D3D11_USAGE_DEFAULT;
  bd4.ByteWidth = sizeof(ShadersLightsParams);
  bd4.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd4.CPUAccessFlags = 0;
  DXEssayer(pD3DDevice->CreateBuffer(&bd4, nullptr, &LightsParamsBuffer));

    // Light Count
  D3D11_BUFFER_DESC bd5;
  ZeroMemory(&bd5, sizeof(bd5));
  bd5.Usage = D3D11_USAGE_DEFAULT;
  bd5.ByteWidth = sizeof(ShadersLightCount);
  bd5.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd5.CPUAccessFlags = 0;
  DXEssayer(pD3DDevice->CreateBuffer(&bd5, nullptr, &LightsCountBuffer));

//////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Pour l'effet
  ID3DBlob* pFXBlob = nullptr;

  DXEssayer(D3DCompileFromFile(path, 0, 0, 0,
    "fx_5_0", 0, 0,
    &pFXBlob, nullptr),
    DXE_ERREURCREATION_FX);

  DXEssayer(D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet));

  pFXBlob->Release();

  pTechnique = pEffet->GetTechniqueByIndex(0);
  pPasse = pTechnique->GetPassByIndex(0);

  // Créer l'organisation des sommets pour le VS de notre effet
  D3DX11_PASS_SHADER_DESC effectVSDesc;
  pPasse->GetVertexShaderDesc(&effectVSDesc);

  D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
  effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

  const void *vsCodePtr = effectVSDesc2.pBytecode;
  const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

  pVertexLayout = nullptr;
  DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
    CSommetBloc::numElements,
    vsCodePtr,
    vsCodeLen,
    &pVertexLayout),
    DXE_CREATIONLAYOUT);

  // Initialisation des paramètres de sampling de la texture
  D3D11_SAMPLER_DESC samplerDesc;

  samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 4;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  // Création de l'état de sampling
  DXEssayer(pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState));
}

void Shader::apply() {
  // Obtenir le contexte
  ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

  // Le sampler state
  ID3DX11EffectSamplerVariable* variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
  variableSampler->SetSampler(0, pSampleState);

  // **** Rendu de l'objet
  pPasse->Apply(0, pImmediateContext);
}

void Shader::passTexture(ID3D11ShaderResourceView* texture) {
  // Activation de la texture
  ID3DX11EffectShaderResourceVariable* variableTexture = pEffet->GetVariableByName("diffuse")->AsShaderResource();
  variableTexture->SetResource(texture);
}

void Shader::passNormal(ID3D11ShaderResourceView* texture) {
  // Activation de la texture
  ID3DX11EffectShaderResourceVariable* variableTexture = pEffet->GetVariableByName("normal")->AsShaderResource();
  variableTexture->SetResource(texture);
}

void Shader::passEmission(ID3D11ShaderResourceView* texture) {
  // Activation de la texture
  ID3DX11EffectShaderResourceVariable* variableTexture = pEffet->GetVariableByName("emission")->AsShaderResource();
  variableTexture->SetResource(texture);
}

Shader::~Shader() {
  DXRelacher(pSampleState);
  DXRelacher(pEffet);
  DXRelacher(pVertexLayout);

  DXRelacher(WorldParamsBuffer);
  DXRelacher(MaterialParamsBuffer);
  DXRelacher(TexturePresenceBuffer);
  DXRelacher(LightsCountBuffer);
  DXRelacher(LightsParamsBuffer);
}