#pragma once
#include "dispositifD3D11.h"
#include "HUD_Manager.h"
#include "AfficheurTexte.h"
#include "MenuInterface.h"
#include "ScreenBillboard.h"
#include "Renderer.h"
#include "Score.h"
#include <vector>
class CScoreboard_Menu : public CMenu
{
    std::vector<std::unique_ptr<CScreenBillboard>> billboard_texte;
    std::vector<std::wstring> text_options;
    std::vector<std::unique_ptr<CAfficheurTexte>> afficheurs;
    ID3D11ShaderResourceView* fond_sc;
    int selectedOption;
public:
    CScoreboard_Menu(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer, CScore *scor, ID3D11ShaderResourceView* fond);
    void afficher();

    void ajoutDernierScore(std::wstring lastEntry);
    void gerer_input(const PM3D::CDIManipulateur& inp);

};