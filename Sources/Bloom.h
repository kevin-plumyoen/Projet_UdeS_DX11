#pragma once

#include "PostFX.h"
#include "RenderTexture.h"
#include "ScreenBillboard.h"


class Bloom : public PostFX {
    PostFX brightness;
    D3D11RenderTexture brightness_texture;
#if 0
    PostFX brightness{ L"Brightness.fx",pDispositif };
    PostFX hblur{ L"HBlur.fx",pDispositif };
    PostFX vblur{ L"VBlur.fx",pDispositif };

    D3D11RenderTexture bloom_texture;
    CScreenBillboard& screen;
#endif
public:
    Bloom(const wchar_t* path, PM3D::CDispositifD3D11* dispositif);
    
    void pre_pass(PrePassContext ctx) override;
    void configure(std::size_t pass_index, ID3DX11Effect& effect) override;

    void on_resize(unsigned int width, unsigned int height);
};
