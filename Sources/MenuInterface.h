#pragma once
#include "AfficheurTexte.h"
#include "Renderer.h"
class CMenu
{
protected:
    Font *pFont;
    PM3D::CDispositifD3D11* pDispositif;
    Renderer *renderer;
public:
    CMenu( Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer)
        : pFont(std::move(fon))
        , pDispositif(std::move(disp))
        , renderer(std::move(gameRenderer)) {}
    virtual void afficher() = 0;
    virtual void gerer_input(const PM3D::CDIManipulateur& ) {};
    virtual void update_vitesse(float) {};
    virtual void update_chrono(int /*min*/, int /*sec*/) {};
};