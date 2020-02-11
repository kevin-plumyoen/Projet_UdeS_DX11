#include "Bloom.h"

#include "util.h"
#include "dispositifD3D11.h"
#include "resource.h"

#include "d3dx11effect.h"

Bloom::Bloom(const wchar_t* path, PM3D::CDispositifD3D11* dispo) 
: PostFX(path, dispo) 
, brightness{L"Brightness.fx", dispo}
, brightness_texture{*dispo} {
}

void Bloom::pre_pass(PrePassContext ctx) {
    // Render scene with brightness
    brightness.render_on_billboard(ctx.screen_billboard, ctx.rendered_screen_texture, ctx.device_context, ctx.swap_render_textures, &brightness_texture);
    
}

void Bloom::configure(std::size_t , ID3DX11Effect& effect) {
    ID3DX11EffectShaderResourceVariable* variableBloom = effect.GetVariableByName("BloomTexture")->AsShaderResource();
    variableBloom->SetResource(brightness_texture.shader_resource_view());
}

void Bloom::on_resize(unsigned int width, unsigned int height) {
    brightness_texture.resize(width, height);
}