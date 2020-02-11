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

class CHUD_En_Jeux : public CMenu
{
    std::vector<std::unique_ptr<CScreenBillboard>> billboard_texte;
    std::vector<std::wstring> text_options;
    std::vector<std::unique_ptr<CAfficheurTexte>> afficheurs;
    int selectedOption;
public:
    CHUD_En_Jeux(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer);
    void afficher();
    void update_chrono(int min, int sec);
    void update_vitesse(float vitesse);
    void gerer_input(const PM3D::CDIManipulateur& inp);
};