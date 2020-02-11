#include "PostFX.h"
#include "util.h"
#include "dispositifD3D11.h"
#include "resource.h"
#include "ScreenBillboard.h"
#include "RenderTexture.h"

#include "d3dx11effect.h"
#include <d3d11.h>
#include <d3dcompiler.h>

static const D3D11_INPUT_ELEMENT_DESC SCREEN_FX_VERTEX_LAYOUT[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

PM3D::CDispositifD3D11& PostFX::dispositf() {
    return *pDispositif;
}

void PostFX::compile_shaders(const wchar_t* path) {
    ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

    ID3DBlob* pFXBlob = nullptr;

    PM3D::DXEssayer(D3DCompileFromFile(path, 0, 0, 0,
                    "fx_5_0", 0, 0,
                    &pFXBlob, nullptr), DXE_ERREURCREATION_FX);

    PM3D::DXEssayer(D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet));

    pFXBlob->Release();

    pTechnique = pEffet->GetTechniqueByIndex(0);

    D3DX11_TECHNIQUE_DESC tech_desc;
    pTechnique->GetDesc(&tech_desc);

    for (uint32_t i = 0; i < tech_desc.Passes; ++i) {
        passes.push_back(pTechnique->GetPassByIndex(i));
    }
}

void PostFX::setup_layout() {
    ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

    D3DX11_PASS_SHADER_DESC effectVSDesc;
    passes.front()->GetVertexShaderDesc(&effectVSDesc);

    D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
    effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

    const void* vsCodePtr = effectVSDesc2.pBytecode;
    const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

    PM3D::DXEssayer(pD3DDevice->CreateInputLayout(SCREEN_FX_VERTEX_LAYOUT, 2, 
                                                  vsCodePtr, vsCodeLen, 
                                                  &pVertexLayout), DXE_CREATIONLAYOUT);
}

void PostFX::setup_sampler() {
    ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

    // Initialisation des paramètres de sampling de la texture
    D3D11_SAMPLER_DESC samplerDesc;

    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
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
    PM3D::DXEssayer(pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState));
}

PostFX::PostFX(const wchar_t* path, PM3D::CDispositifD3D11* dispositif)
: pDispositif{dispositif}
, pEffet{}
, pTechnique{}
, passes{}
, pVertexLayout{} {
    compile_shaders(path);
    setup_layout();
    setup_sampler();
}

PostFX::~PostFX() {
    PM3D::DXRelacher(pVertexLayout);
    for(int i = 0; i < passes.size(); ++i) {
        PM3D::DXRelacher(passes[i]);
    }

    PM3D::DXRelacher(pTechnique);
    PM3D::DXRelacher(pEffet);
}

std::size_t PostFX::pass_count() const noexcept {
    return passes.size();
}

void PostFX::configure(std::size_t, ID3DX11Effect&) {
    // Does nothing by default
}

void PostFX::pre_pass(PrePassContext /*ctx*/) {
    // Does nothing by default
}

void PostFX::apply_pass(std::size_t pass_index, ID3D11ShaderResourceView* screen_texture) {
    // Configure les paramètres obligatoires
    ID3DX11EffectShaderResourceVariable* variableTexture = pEffet->GetVariableByName("RenderedFrameTexture")->AsShaderResource();
    variableTexture->SetResource(screen_texture);

    ID3DX11EffectSamplerVariable* variableSampler = pEffet->GetVariableByName("RenderedFrameSampler")->AsSampler();
    variableSampler->SetSampler(0, pSampleState);

    configure(pass_index, *pEffet);
    
    ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
    passes[pass_index]->Apply(0, pImmediateContext);
    pImmediateContext->IASetInputLayout(pVertexLayout);
}

void PostFX::render_on_billboard(CScreenBillboard& screen_billboard, ID3D11ShaderResourceView* rendered_frame_texture, ID3D11DeviceContext* ctx, D3D11RenderTexture* render_textures) {
    ID3D11ShaderResourceView* previous_texture = rendered_frame_texture;

    for (int pass_index = 0; pass_index < pass_count(); ++pass_index) {
        const int render_target_index = pass_index % 2;
        D3D11RenderTexture& effect_render_target = render_textures[render_target_index];

        effect_render_target.use_as_target();
        effect_render_target.clear();

        apply_pass(pass_index, previous_texture);
        screen_billboard.Draw();

        previous_texture = effect_render_target.shader_resource_view();

        // Set shader resource view to null
        ID3D11ShaderResourceView* null_resource_view[] = { nullptr, nullptr };
        ctx->PSSetShaderResources(0, 2, null_resource_view);
    }
}

void PostFX::render_on_billboard(CScreenBillboard& screen_billboard, ID3D11ShaderResourceView* rendered_frame_texture, ID3D11DeviceContext* ctx, D3D11RenderTexture* render_textures, D3D11RenderTexture* dest_render_texture) {
    ID3D11ShaderResourceView* previous_texture = rendered_frame_texture;

    for (int pass_index = 0; pass_index < pass_count(); ++pass_index) {
        const int render_target_index = pass_index % 2;
        D3D11RenderTexture* effect_render_target = render_textures + render_target_index;

        // Last pass is made into destination render texture
        if (pass_index == pass_count() - 1) {
            effect_render_target = dest_render_texture;
        }

        effect_render_target->use_as_target();
        effect_render_target->clear();

        apply_pass(pass_index, previous_texture);
        screen_billboard.Draw();

        previous_texture = effect_render_target->shader_resource_view();

        // Set shader resource view to null
        ID3D11ShaderResourceView* null_resource_view[] = { nullptr, nullptr };
        ctx->PSSetShaderResources(0, 2, null_resource_view);
    }
}

void PostFX::clearRenderTarger() {
    pDispositif->ResetTargetView();
}

ID3D11RenderTargetView* PostFX::getRenderTarger() {
    return pDispositif->GetRenderTargetView();
}