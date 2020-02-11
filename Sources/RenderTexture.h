#ifndef DEF_RENDER_TEXTURE_H
#define DEF_RENDER_TEXTURE_H

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

namespace PM3D {
    class CDispositifD3D11;
}

class D3D11RenderTexture {
    PM3D::CDispositifD3D11& dispositif;
    ID3D11Texture2D* texture;
    ID3D11RenderTargetView* target_view;
    ID3D11ShaderResourceView* shader_view;
    ID3D11Texture2D* depth;
    ID3D11DepthStencilView* depth_stencil_view;

    void create_texture(unsigned int width, unsigned int height);
    void create_depth_texture(unsigned int width, unsigned int height);
    void create_render_target();
    void create_depth_shader_resource();
    void create_shader_resource();

    void adapt_to_size(unsigned int width, unsigned int height);
public:
    D3D11RenderTexture(PM3D::CDispositifD3D11& dispositif);
    D3D11RenderTexture(const D3D11RenderTexture&) = delete;
    D3D11RenderTexture& operator=(const D3D11RenderTexture&) = delete;
    ~D3D11RenderTexture();

    void clear();
    void resize(unsigned int width, unsigned int height);
    void use_as_target();
    void swap(D3D11RenderTexture& texture) noexcept;

    ID3D11ShaderResourceView* shader_resource_view() const noexcept { return shader_view; }
};

#endif