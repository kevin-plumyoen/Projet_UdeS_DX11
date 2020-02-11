#ifndef DEF_TEXT_COMPONENT_H
#define DEF_TEXT_COMPONENT_H

#include "ScreenBillboard.h"
#include "BillboardComponent.h"
#include "Materiel.h"
#include <memory>
#include <string.h>
#include <string>
#include <Gdiplus.h>

#pragma comment(lib, "gdiplus.lib")


struct TextComponent {
    std::shared_ptr<CScreenBillboard> billboard;
    std::shared_ptr<Materiel> material;//texture 2d
    BillboardLinearOrientation orientation;
    UINT TexWidth;
    UINT TexHeight;

    Gdiplus::Font* pFont;
    std::unique_ptr<Gdiplus::Bitmap> pCharBitmap;
    std::unique_ptr<Gdiplus::Graphics> pCharGraphics;
    std::unique_ptr<Gdiplus::SolidBrush>pBlackBrush;

    static ULONG_PTR token;
};

#endif