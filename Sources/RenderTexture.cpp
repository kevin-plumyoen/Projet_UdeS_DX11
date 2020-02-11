#include "RenderTexture.h"
#include "dispositifD3D11.h"
#include "util.h"
#include <algorithm>
#include <d3d11.h>

void D3D11RenderTexture::create_texture(unsigned int width, unsigned int height) {
    if (texture) {
        PM3D::DXRelacher(texture);
    }

    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof texture_desc);

    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    PM3D::DXEssayer(dispositif.GetD3DDevice()->CreateTexture2D(&texture_desc, NULL, &texture));
}

void D3D11RenderTexture::create_depth_texture(unsigned int width, unsigned int height) {
    if (depth) {
        PM3D::DXRelacher(depth);
    }

    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof texture_desc);

    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    PM3D::DXEssayer(dispositif.GetD3DDevice()->CreateTexture2D(&texture_desc, NULL, &depth));
}

void D3D11RenderTexture::create_render_target() {
    if (target_view) {
        PM3D::DXRelacher(target_view);
    }

    D3D11_RENDER_TARGET_VIEW_DESC desc;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;

    PM3D::DXEssayer(dispositif.GetD3DDevice()->CreateRenderTargetView(texture, &desc, &target_view));
}

void D3D11RenderTexture::create_depth_shader_resource() {
    if (depth_stencil_view) {
        PM3D::DXRelacher(depth_stencil_view);
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
    ZeroMemory( &descDSView, sizeof( descDSView ) );
    descDSView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSView.Texture2D.MipSlice = 0;
    PM3D::DXEssayer(dispositif.GetD3DDevice()->CreateDepthStencilView(depth, &descDSView, &depth_stencil_view));
}

void D3D11RenderTexture::create_shader_resource() {
    if (shader_view) {
        PM3D::DXRelacher(shader_view);
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MostDetailedMip = 0;
    desc.Texture2D.MipLevels = 1;

    PM3D::DXEssayer(dispositif.GetD3DDevice()->CreateShaderResourceView(texture, &desc, &shader_view));
}

void D3D11RenderTexture::adapt_to_size(unsigned int width, unsigned int height) {
    create_texture(width, height);
    create_depth_texture(width, height);
    create_render_target();
    create_shader_resource();
    create_depth_shader_resource();
}

D3D11RenderTexture::D3D11RenderTexture(PM3D::CDispositifD3D11& dispositif)
: dispositif{dispositif}
, texture{nullptr}
, target_view{nullptr}
, shader_view{nullptr}
, depth{nullptr}
, depth_stencil_view{nullptr} {

}

D3D11RenderTexture::~D3D11RenderTexture() {
    PM3D::DXRelacher(depth);
    PM3D::DXRelacher(depth_stencil_view);
    PM3D::DXRelacher(shader_view);
    PM3D::DXRelacher(target_view);
    PM3D::DXRelacher(texture);
}

void D3D11RenderTexture::clear() {
    ID3D11DeviceContext* pImmediateContext = dispositif.GetImmediateContext();

    // On efface la surface de rendu
    float Couleur[4] = { 0.0f, 0.5f, 0.0f, 1.0f };  //  RGBA - Vert pour le moment
    pImmediateContext->ClearRenderTargetView(target_view, Couleur);

    // On ré-initialise le tampon de profondeur
    pImmediateContext->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D11RenderTexture::resize(unsigned int width, unsigned int height) {
    adapt_to_size(width, height);
}

void D3D11RenderTexture::use_as_target() {
    dispositif.SetRenderTargetView(target_view, depth_stencil_view);
}

void D3D11RenderTexture::swap(D3D11RenderTexture& other_texture) noexcept {
    using std::swap;

    swap(depth, other_texture.depth);
    swap(depth_stencil_view, other_texture.depth_stencil_view);
    swap(shader_view, other_texture.shader_view);
    swap(target_view, other_texture.target_view);
    swap(texture, other_texture.texture);
}