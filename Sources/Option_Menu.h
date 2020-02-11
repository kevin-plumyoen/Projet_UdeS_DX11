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

class COption_Menu : public CMenu
{
    std::vector<std::unique_ptr<CScreenBillboard>> billboard_texte;
    std::vector<std::wstring> text_options;
    std::vector<std::unique_ptr<CAfficheurTexte>> afficheurs;
    std::chrono::system_clock::time_point currentTime, pastTime;
    ID3D11ShaderResourceView* fon_opt;
    int selectedOption;
public:
    COption_Menu(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer,ID3D11ShaderResourceView* fond);
    void afficher();
    void changeSelection();
    void changerOption();
    void selectionner();
    void gerer_input(const PM3D::CDIManipulateur& inp);
};