#include "Scoreboard_Menu.h"

CScoreboard_Menu::CScoreboard_Menu(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer, CScore *scor, ID3D11ShaderResourceView* fond)
    : CMenu(std::move(fon), std::move(disp), std::move(gameRenderer))
    , text_options{ scor->getText() }
    ,fond_sc{fond}
{
    selectedOption = 0;
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,-0.5f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,-0.4f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.3f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.4f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.5f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.6f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.7f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.8f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.9f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,1.0f }, Vec2{ .1f, .2f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.f }, Vec2{ 1.f, 1.f }, pDispositif));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    std::reverse(text_options.begin(), text_options.end());
}
void CScoreboard_Menu::afficher() {
    renderer->push_hud_command(RenderHudObjectCommand(billboard_texte.at(10).get(), fond_sc));
    for (int i = 0; i < text_options.size(); ++i)
    {
        renderer->push_hud_command(RenderHudObjectCommand(billboard_texte.at(i).get(), afficheurs.at(i)->GetTextureView()));
        afficheurs.at(i)->Ecrire(text_options.at(i), *pFont);
    }
}

void CScoreboard_Menu::ajoutDernierScore(std::wstring lastEntry)
{
    text_options.emplace_back(lastEntry);
}

void CScoreboard_Menu::gerer_input(const PM3D::CDIManipulateur& )
{
    /*pastTime = std::chrono::system_clock::now();
    if ((pastTime - currentTime) > std::chrono::milliseconds(75))
    {
        if (inp.ToucheAppuyee(KeyboardKey::s))
            changeSelection();
        if (inp.ToucheAppuyee(KeyboardKey::enter))
            selectionner();
        if (inp.ToucheAppuyee(KeyboardKey::escape))
            EventBus::instance().transmit(HUDEvent(HUDEvent::Event::MenuPrecedent));
        currentTime = pastTime;
    }*/
}