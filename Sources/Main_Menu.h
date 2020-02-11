#pragma once
#include "dispositifD3D11.h"
#include "HUD_Manager.h"
#include "AfficheurTexte.h"
#include "MenuInterface.h"
#include "ScreenBillboard.h"
#include "Renderer.h"
#include "EventBus.h"
#include "HUD_Event.h"
#include <vector>
#include <chrono>
class CMain_Menu : public CMenu
{
    std::vector<std::unique_ptr<CScreenBillboard>> billboard_texte;
    std::vector<std::wstring> text_options;
    std::vector<std::unique_ptr<CAfficheurTexte>> afficheurs;
    ID3D11ShaderResourceView* fond_men;
    std::chrono::system_clock::time_point currentTime, pastTime;
    int selectedOption;
public:
    CMain_Menu(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer, ID3D11ShaderResourceView* fond);
    void afficher();
    void changeSelection();
    void selectionner();

    void gerer_input(const PM3D::CDIManipulateur& inp);
};