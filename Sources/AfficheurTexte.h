#pragma once
#pragma comment(lib,"gdiplus.lib")

#include <string>
#include <memory>

namespace PM3D {
    class CDispositifD3D11;
}

struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct IDXGISurface1;

namespace Gdiplus {
    class Graphics;
    class Font;
    class SolidBrush;
    class Bitmap;
    class FontFamily;
}

class Font;

class FontContext {
    FontContext();
    ~FontContext();
public:
    static FontContext& get();
};

class CAfficheurTexte
{
public :
    CAfficheurTexte(PM3D::CDispositifD3D11* pDispositif, int largeur, int hauteur, FontContext& ctx);
    ~CAfficheurTexte();
    void Ecrire(const std::wstring &s, const Font& font);
    ID3D11ShaderResourceView* GetTextureView() { return pTextureView; }
private:
    FontContext& ctx;
    struct Impl;
    std::unique_ptr<Impl> pimpl;
    ID3D11Texture2D* pTexture;
    ID3D11ShaderResourceView* pTextureView;
    PM3D::CDispositifD3D11* pDispo;

    unsigned int TexWidth;
    unsigned int TexHeight;
};

class Font {
    friend CAfficheurTexte;
    struct Impl;
    std::unique_ptr<Impl> pimpl;
public:
    enum class Style {
        Regular,
        Bold,
        Italic,
        BoldItalic,
        Underline,
    };

    Font(std::wstring_view name, float size, Style style = Style::Regular);
    ~Font();
};