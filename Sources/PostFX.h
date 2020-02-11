#ifndef DEF_POSTFX_H
#define DEF_POSTFX_H

#include <vector>

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectPass;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DeviceContext;

class CScreenBillboard;
class D3D11RenderTexture;

namespace PM3D {
    class CDispositifD3D11;
}

struct PrePassContext {
    CScreenBillboard& screen_billboard;
    ID3D11ShaderResourceView* rendered_screen_texture;
    D3D11RenderTexture* swap_render_textures;
    ID3D11DeviceContext* device_context;

    PrePassContext(CScreenBillboard& screen_billboard, ID3D11ShaderResourceView* rendered_screen_texture, D3D11RenderTexture* swap_render_textures, ID3D11DeviceContext* device_context)
    : screen_billboard{ screen_billboard }
    , rendered_screen_texture{ rendered_screen_texture }
    , swap_render_textures{swap_render_textures}
    , device_context{device_context} {

    }
};

class PostFX {
private:
    PM3D::CDispositifD3D11* pDispositif;
    ID3DX11Effect* pEffet;
    ID3DX11EffectTechnique* pTechnique;
    std::vector<ID3DX11EffectPass*> passes;

    ID3D11InputLayout* pVertexLayout;
    ID3D11SamplerState* pSampleState;

    void compile_shaders(const wchar_t* path);
    void setup_layout();
    void setup_sampler();

    void clearRenderTarger();
    ID3D11RenderTargetView* getRenderTarger();
protected:
    PM3D::CDispositifD3D11& dispositf();
public:
    PostFX(const wchar_t* path, PM3D::CDispositifD3D11* dispositif);
    ~PostFX();

    virtual void configure(std::size_t pass_index, ID3DX11Effect& effect);
    virtual void pre_pass(PrePassContext ctx);

    std::size_t pass_count() const noexcept;
    void apply_pass(std::size_t pass_index, ID3D11ShaderResourceView* screen_texture);
    
    void render_on_billboard(CScreenBillboard&, ID3D11ShaderResourceView*, ID3D11DeviceContext*, D3D11RenderTexture*);
    void render_on_billboard(CScreenBillboard&, ID3D11ShaderResourceView*, ID3D11DeviceContext*, D3D11RenderTexture*, D3D11RenderTexture*);

    // Copy is disabled
    PostFX(const PostFX&) = delete;
    PostFX& operator=(const PostFX&) = delete;
};

#endif