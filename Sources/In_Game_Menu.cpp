#include "In_Game_Menu.h"

CIn_Game_Menu::CIn_Game_Menu(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer, ID3D11ShaderResourceView* fond)
    : CMenu(std::move(fon), std::move(disp), std::move(gameRenderer))
    , text_options{ L"->Resume", L"  Options", L"  Exit" }
    , fond_men{ fond }
{
    selectedOption = 2;
    currentTime = std::chrono::system_clock::now();
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,-0.4f }, Vec2{ .5f, .4f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,-0.2f }, Vec2{ .5f, .4f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.0f }, Vec2{ .5f, .4f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.f }, Vec2{ 1.f, 1.f }, pDispositif));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    std::reverse(text_options.begin(), text_options.end());
}
void CIn_Game_Menu::afficher() {
    renderer->push_hud_command(RenderHudObjectCommand(billboard_texte.at(3).get(), fond_men));
    for (int i = 0; i < afficheurs.size(); ++i)
    {
        renderer->push_hud_command(RenderHudObjectCommand(billboard_texte.at(i).get(), afficheurs.at(i)->GetTextureView()));
        afficheurs.at(i)->Ecrire(text_options.at(i), *pFont);
    }
}
void CIn_Game_Menu::changeSelection()
{
    text_options.at(selectedOption) = L"  " + text_options.at(selectedOption).erase(0, 2);
    selectedOption = (((--selectedOption>-1) ? selectedOption : 2) % afficheurs.size());
    text_options.at(selectedOption).erase(0, 2);
    text_options.at(selectedOption) = L"->" + text_options.at(selectedOption);
}

void CIn_Game_Menu::selectionner()
{
    switch (selectedOption)
    {
    case 0:
        EventBus::instance().transmit(HUDEvent(HUDEvent::Event::MenuQuitter));
        break;
    case 1:
        EventBus::instance().transmit(HUDEvent(HUDEvent::Event::MenuOption));
        break;
    case 2:
        EventBus::instance().transmit(HUDEvent(HUDEvent::Event::MenuReprendre));
        break;
    }
}
void CIn_Game_Menu::gerer_input(const PM3D::CDIManipulateur& inp)
{
    pastTime = std::chrono::system_clock::now();
    if ((pastTime - currentTime) > std::chrono::milliseconds(75))
    {
        if (inp.ToucheAppuyee(KeyboardKey::down))
            changeSelection();
        if (inp.ToucheAppuyee(KeyboardKey::enter))
            selectionner();
        if (inp.ToucheAppuyee(KeyboardKey::escape))
            EventBus::instance().transmit(HUDEvent(HUDEvent::Event::MenuReprendre));
        currentTime = pastTime;
    }
}