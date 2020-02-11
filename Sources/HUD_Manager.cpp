#include "HUD_Manager.h"

void CHUD_Manager::changeCurrentMenu(std::unique_ptr<CMenu> element) {
    element_menu_precedent = std::move(element_menu_courrant);
    element_menu_courrant = std::move(element); 
}
void CHUD_Manager::afficher_menu() { element_menu_courrant->afficher(); }
void CHUD_Manager::swap_menus() { 
    element_menu_courrant = std::move(element_menu_precedent);
}
void CHUD_Manager::afficher_menu_precedent() { element_menu_precedent->afficher(); }
void CHUD_Manager::gerer_input_menu_courrant(const PM3D::CDIManipulateur& inp) {element_menu_courrant->gerer_input(inp); }
void CHUD_Manager::update_game_hud_vitesse(float vit) { element_menu_courrant->update_vitesse(vit); }
void CHUD_Manager::update_game_hud_chrono(int min, int sec) { element_menu_courrant->update_chrono(min, sec); }