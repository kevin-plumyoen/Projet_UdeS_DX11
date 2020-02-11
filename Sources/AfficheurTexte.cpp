#include "AfficheurTexte.h"
#include "util.h"
#include "dispositifD3D11.h"

#pragma warning(push, 0)  
#include <Gdiplus.h>
#pragma warning(pop)

static ULONG_PTR g_token = 0;

FontContext::FontContext() {
    Gdiplus::GdiplusStartupInput startupInput(0, TRUE, TRUE);
    Gdiplus::GdiplusStartupOutput startupOutput;

    GdiplusStartup(&g_token, &startupInput, &startupOutput);
}

FontContext::~FontContext() {
    Gdiplus::GdiplusShutdown(g_token);
}

FontContext& FontContext::get() {
    static FontContext instance;
    return instance;
}

static int to_gdi_style(Font::Style style) {
    switch (style) {
    case Font::Style::Regular:
        return Gdiplus::FontStyleRegular;
    case Font::Style::Bold:
        return Gdiplus::FontStyleBold;
    case Font::Style::Italic:
        return Gdiplus::FontStyleItalic;
    case Font::Style::BoldItalic:
        return Gdiplus::FontStyleBoldItalic;
    case Font::Style::Underline:
        return Gdiplus::FontStyleUnderline;
    }

    return Gdiplus::FontStyleRegular;
}

struct Font::Impl {
    std::unique_ptr<Gdiplus::FontFamily> family;
    std::unique_ptr<Gdiplus::Font> font;
    Impl(std::wstring_view name, float size, Style style)
    : family(std::make_unique<Gdiplus::FontFamily>(name.data(), nullptr))
    , font(std::make_unique<Gdiplus::Font>(family.get(), size, to_gdi_style(style), Gdiplus::UnitPixel)) {

    }
};

Font::Font(std::wstring_view name, float size, Style style) 
: pimpl{std::make_unique<Impl>(name, size, style)}{

}

Font::~Font() = default;

struct CAfficheurTexte::Impl {
    struct BrushDeleter {
        void operator()(Gdiplus::SolidBrush* ptr) const noexcept {
            ::delete ptr;
        }
    };

    std::unique_ptr<Gdiplus::Bitmap> pCharBitmap;
    std::unique_ptr<Gdiplus::Graphics> pCharGraphics;
    std::unique_ptr<Gdiplus::SolidBrush> pBlackBrush;

    static std::unique_ptr<Gdiplus::Graphics> make_graphics(Gdiplus::Bitmap* bitmap) {
        std::unique_ptr<Gdiplus::Graphics> graphics = std::make_unique<Gdiplus::Graphics>(bitmap);
        graphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
        graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
        graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighSpeed);
        graphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);
        graphics->SetPageUnit(Gdiplus::UnitPixel);
        Gdiplus::TextRenderingHint hint = Gdiplus::TextRenderingHintAntiAlias;

        graphics->SetTextRenderingHint(hint);

        return graphics;
    }

    Impl(int largeur, int hauteur)
    : pCharBitmap{std::make_unique<Gdiplus::Bitmap>(largeur, hauteur, PixelFormat32bppARGB)}
    , pCharGraphics{make_graphics(pCharBitmap.get())}
    , pBlackBrush{new Gdiplus::SolidBrush{Gdiplus::Color(255, 255, 255, 255)}} {

    }
};

CAfficheurTexte::CAfficheurTexte(PM3D::CDispositifD3D11* pDispositif, int largeur, int hauteur, FontContext& ctx)
: ctx{ctx}
, pimpl{std::make_unique<Impl>(largeur, hauteur)}
, pDispo(pDispositif)
, pTexture(nullptr)
, pTextureView(nullptr)
, TexWidth(largeur)
, TexHeight(hauteur) {
    pimpl->pCharGraphics->Clear(Gdiplus::Color(0,0,0, 0));
    Gdiplus::BitmapData bmData;
    auto rect = Gdiplus::Rect(0, 0, TexWidth, TexHeight);
    pimpl->pCharBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width = TexWidth;
    texDesc.Height = TexHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = bmData.Scan0;
    data.SysMemPitch = TexWidth * 4;
    data.SysMemSlicePitch = 0;

    PM3D::DXEssayer(pDispo->GetD3DDevice()->CreateTexture2D(&texDesc, &data, &pTexture));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    PM3D::DXEssayer(pDispositif->GetD3DDevice()->CreateShaderResourceView(pTexture, &srvDesc, &pTextureView));

    pimpl->pCharBitmap->UnlockBits(&bmData);
}

CAfficheurTexte::~CAfficheurTexte()
{
    PM3D::DXRelacher(pTextureView);
    PM3D::DXRelacher(pTexture);
}

void CAfficheurTexte::Ecrire(const std::wstring& s, const Font& font)
{
    pimpl->pCharGraphics->Clear(Gdiplus::Color(0, 0, 0, 0));

    pimpl->pCharGraphics->DrawString(s.c_str(), 
                                     static_cast<int>(s.size()), 
                                     font.pimpl->font.get(), 
                                     Gdiplus::PointF(0.0f, 0.0f), 
                                     pimpl->pBlackBrush.get());

    Gdiplus::BitmapData bmData;
    auto rect = Gdiplus::Rect(0, 0, TexWidth, TexHeight);

    pimpl->pCharBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);
    pDispo->GetImmediateContext()->UpdateSubresource(pTexture, 0, 0, bmData.Scan0, TexWidth * 4, 0);
    pimpl->pCharBitmap->UnlockBits(&bmData);
}