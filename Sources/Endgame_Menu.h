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
#include <algorithm>
#include <cmath>
#include <sstream>

class CEndgame_Menu : public CMenu
{
    std::vector<std::unique_ptr<CScreenBillboard>> billboard_texte;
    std::vector<std::wstring> text_options;
    std::vector<std::unique_ptr<CAfficheurTexte>> afficheurs;
    std::chrono::system_clock::time_point currentTime, pastTime;
    ID3D11ShaderResourceView* fond_men;
    bool temps = false;

public:
    CEndgame_Menu(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer, ID3D11ShaderResourceView* fond_men);
    void afficher();
    void gerer_input(const PM3D::CDIManipulateur& inp);
    void update_chrono(int min, int sec);
};