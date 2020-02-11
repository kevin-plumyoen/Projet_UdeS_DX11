#pragma once
#include "MenuInterface.h"
#include "DIManipulateur.h"

class CHUD_Manager
{
    std::unique_ptr<CMenu> element_menu_courrant;
    std::unique_ptr<CMenu> element_menu_precedent;
public:
    CHUD_Manager() = default;
    void changeCurrentMenu(std::unique_ptr<CMenu> element);
    void afficher_menu();
    void afficher_menu_precedent();
    void swap_menus();
    void gerer_input_menu_courrant(const PM3D::CDIManipulateur& inp);
    void update_game_hud_vitesse(float vit);
    void update_game_hud_chrono(int min, int sec);
    //~CHUD_Manager() { delete(element_menu_courrant); }
};