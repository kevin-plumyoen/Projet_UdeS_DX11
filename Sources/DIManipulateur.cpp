#include "DIManipulateur.h"
#include "util.h"
#include "resource.h"
#include "DInputInitData.h"
#include "Logger.h"
#include <cassert>
#include <dinput.h>
#include <stdexcept>
#include <algorithm>
#include <stdexcept>

namespace PM3D
{
    struct CDIManipulateur::Impl {
        IDirectInput8 * pDirectInput;
        IDirectInputDevice8* pClavier;
        IDirectInputDevice8* pSouris;
        IDirectInputDevice8* pJoystick;
        DIMOUSESTATE mouseState;
        HWND hwnd;
#ifndef NEDITOR
        bool is_cursor_locked_;
#endif

        Impl()
        : pDirectInput{}
        , pClavier{}
        , pSouris{}
        , pJoystick{} 
#ifndef NEDITOR
        , is_cursor_locked_{true} 
#endif
        {
        }

        ~Impl() {
            if (pClavier)
            {
                pClavier->Unacquire();
                pClavier->Release();
                pClavier = nullptr;
            }

            if (pSouris)
            {
                pSouris->Unacquire();
                pSouris->Release();
                pSouris = nullptr;
            }

            if (pJoystick)
            {
                pJoystick->Unacquire();
                pJoystick->Release();
                pJoystick = nullptr;
            }

            if (pDirectInput)
            {
                pDirectInput->Release();
                pClavier = nullptr;
            }
        }

        RECT client_space_rectangle() const {
            RECT client_rect;
            if (GetClientRect(hwnd, &client_rect) == FALSE) {
                throw std::system_error{std::error_code{static_cast<int>(GetLastError()), std::system_category()}, 
                                        "failed to get window's client rectangle"};
            }

            // Converti l'espace client vers l'espace écran
            RECT window_rect = client_rect;
            if (ClientToScreen(hwnd, reinterpret_cast<POINT*>(&window_rect.left)) == FALSE) {
                throw std::system_error{std::error_code{static_cast<int>(GetLastError()), std::system_category()}, 
                                        "failed to convert window's client rectangle to screen space"};
            }

            if (ClientToScreen(hwnd, reinterpret_cast<POINT*>(&window_rect.right)) == FALSE) {
                throw std::system_error{std::error_code{static_cast<int>(GetLastError()), std::system_category()}, 
                                        "failed to convert window's client rectangle to screen space"};
            }

            return window_rect;
        }

        void restrict_mouse_cursor_to_rect(const RECT& rect) {
#ifndef NEDITOR
            if(is_cursor_locked_) {
#endif
                if (ClipCursor(&rect) == FALSE) {
                    throw std::system_error{std::error_code{static_cast<int>(GetLastError()), std::system_category()}, 
                                            "failed to clip mouse cursor to screen rectangle"};
                }
#ifndef NEDITOR
            }
#endif
        }

        void unlock_mouse_cursor() {
#ifndef NEDITOR
            is_cursor_locked_ = false;
#endif
            if (ClipCursor(NULL) == FALSE) {
                    throw std::system_error{std::error_code{static_cast<int>(GetLastError()), std::system_category()}, 
                                            "failed to unclip mouse cursor to screen rectangle"};
            }
        }
    };

    bool CDIManipulateur::bDejaInit = false;

    CDIManipulateur::CDIManipulateur()
    : pimpl(std::make_unique<Impl>())
    {
    }

    CDIManipulateur::~CDIManipulateur() = default;

    bool CDIManipulateur::Init(init_data init)
    {
        const DInputInitData* init_data = reinterpret_cast<const DInputInitData*>(init);

        // Un seul objet DirectInput est permis
        if (!bDejaInit)
        {
            // Objet DirectInput
            DXEssayer(DirectInput8Create(init_data->hInstance,
                DIRECTINPUT_VERSION,
                IID_IDirectInput8,
                (void**)&pimpl->pDirectInput,
                nullptr), ERREUR_CREATION_DIRECTINPUT);

            // Objet Clavier
            DXEssayer(pimpl->pDirectInput->CreateDevice(GUID_SysKeyboard,
                &pimpl->pClavier,
                nullptr),
                ERREUR_CREATION_CLAVIER);

            DXEssayer(pimpl->pClavier->SetDataFormat(&c_dfDIKeyboard),
                ERREUR_CREATION_FORMATCLAVIER);

            pimpl->pClavier->SetCooperativeLevel(init_data->hWnd,
                DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

            pimpl->pClavier->Acquire();

            // Objet Souris
            DXEssayer(pimpl->pDirectInput->CreateDevice(GUID_SysMouse, &pimpl->pSouris, nullptr),
                ERREUR_CREATION_SOURIS);

            DXEssayer(pimpl->pSouris->SetDataFormat(&c_dfDIMouse), ERREUR_CREATION_FORMATSOURIS);

            pimpl->pSouris->SetCooperativeLevel(init_data->hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

            pimpl->pSouris->Acquire();

            // Objet Joystick

            bDejaInit = true;
        }

        return true;
    }

    void CDIManipulateur::StatutClavier()
    {
        assert(bDejaInit);
        pimpl->pClavier->GetDeviceState(sizeof(tamponClavier), (void*)& tamponClavier);
    }

    constexpr UINT key_to_dinput(KeyboardKey key) {
        switch (key) {
        case KeyboardKey::a:
            return DIK_A;
        case KeyboardKey::b:
            return DIK_B;
        case KeyboardKey::c:
            return DIK_C;
        case KeyboardKey::d:
            return DIK_D;
        case KeyboardKey::e:
            return DIK_E;
        case KeyboardKey::f:
            return DIK_F;
        case KeyboardKey::g:
            return DIK_G;
        case KeyboardKey::h:
            return DIK_H;
        case KeyboardKey::i:
            return DIK_I;
        case KeyboardKey::j:
            return DIK_J;
        case KeyboardKey::k:
            return DIK_K;
        case KeyboardKey::l:
            return DIK_L;
        case KeyboardKey::m:
            return DIK_M;
        case KeyboardKey::n:
            return DIK_N;
        case KeyboardKey::o:
            return DIK_O;
        case KeyboardKey::p:
            return DIK_P;
        case KeyboardKey::q:
            return DIK_Q;
        case KeyboardKey::r:
            return DIK_R;
        case KeyboardKey::s:
            return DIK_S;
        case KeyboardKey::t:
            return DIK_T;
        case KeyboardKey::u:
            return DIK_U;
        case KeyboardKey::v:
            return DIK_V;
        case KeyboardKey::w:
            return DIK_W;
        case KeyboardKey::x:
            return DIK_X;
        case KeyboardKey::y:
            return DIK_Y;
        case KeyboardKey::z:
            return DIK_Z;
        case KeyboardKey::space:
            return DIK_SPACE;
        case KeyboardKey::pause:
            return DIK_PAUSE;
        case KeyboardKey::enter:
            return DIK_RETURN;
        case KeyboardKey::escape:
            return DIK_ESCAPE;
        case KeyboardKey::up:
            return DIK_UP;
        case KeyboardKey::down:
            return DIK_DOWN;
        case KeyboardKey::f1:
            return DIK_F1;
        case KeyboardKey::f2:
            return DIK_F2;
        case KeyboardKey::f3:
            return DIK_F3;
        case KeyboardKey::f4:
            return DIK_F4;
        case KeyboardKey::f5:
            return DIK_F5;
        case KeyboardKey::f6:
            return DIK_F6;
        case KeyboardKey::f7:
            return DIK_F7;
        case KeyboardKey::f8:
            return DIK_F8;
        case KeyboardKey::f9:
            return DIK_F9;
        case KeyboardKey::f10:
            return DIK_F10;
        case KeyboardKey::f11:
            return DIK_F11;
        case KeyboardKey::f12:
            return DIK_F12;
        case KeyboardKey::num_1:
            return DIK_1;
        case KeyboardKey::num_2:
            return DIK_2;
        case KeyboardKey::num_3:
            return DIK_3;
        case KeyboardKey::num_4:
            return DIK_4;
        case KeyboardKey::num_5:
            return DIK_5;
        case KeyboardKey::num_6:
            return DIK_6;
        case KeyboardKey::num_7:
            return DIK_7;
        case KeyboardKey::num_8:
            return DIK_8;
        case KeyboardKey::num_9:
            return DIK_9;
        case KeyboardKey::num_0:
            return DIK_0;
        default:
            throw std::out_of_range("key not supported yet");
        }
    }

    bool CDIManipulateur::ToucheAppuyee(KeyboardKey touche_) const
    {
        const UINT touche = key_to_dinput(touche_);
        assert(bDejaInit);
        return (tamponClavier[touche] & 0x80);
    }

    void CDIManipulateur::SaisirEtatSouris()
    {
        assert(bDejaInit);
        pimpl->pSouris->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&pimpl->mouseState);
    }

    void CDIManipulateur::set_window_handle(const void* window_handle) {
        pimpl->hwnd = *reinterpret_cast<const HWND*>(window_handle);
    }

    MouseState CDIManipulateur::EtatSouris() const {
        return MouseState(pimpl->mouseState.lX, pimpl->mouseState.lY, pimpl->mouseState.lZ, 
                          pimpl->mouseState.rgbButtons[0], pimpl->mouseState.rgbButtons[1]);
    }

    Vec2 CDIManipulateur::normalized_mouse_coords() const {
        POINT cursor_pos;
        if (screen_width > 0 && screen_height > 0 
         && GetCursorPos(&cursor_pos) && ScreenToClient(pimpl->hwnd, &cursor_pos)) {
            const float half_width = screen_width / 2.f;
            const float half_height = screen_height / 2.f;
        
            const int clamped_cursor_x = std::clamp(static_cast<int>(cursor_pos.x), 0, screen_width);
            const int clamped_cursor_y = std::clamp(static_cast<int>(cursor_pos.y), 0, screen_height);

            const float normalized_x = (clamped_cursor_x - half_width) / half_width;
            const float normalized_y = (clamped_cursor_y - half_height) / half_height;

            assert(std::abs(normalized_x) <= 1.f);
            assert(std::abs(normalized_y) <= 1.f);

            return Vec2{normalized_x, normalized_y * -1.f};
        }

        return Vec2{0.f, 0.f};
    }

    void CDIManipulateur::update_screen_size(int new_width, int new_height) {
        screen_width = new_width;
        screen_height = new_height;

        // Récupère l'espace client de la fenêtre du jeu
        try {
            pimpl->restrict_mouse_cursor_to_rect(pimpl->client_space_rectangle());
        }
        catch (const std::system_error& err) {
            log_critical("engine.input.update_screen_size", err.what());
        }
    }

    void CDIManipulateur::hide_cursor() {
        ShowCursor(FALSE);
    }

    void CDIManipulateur::show_cursor() {
        ShowCursor(TRUE);
    }

#ifndef NEDITOR
    void CDIManipulateur::unlock_cursor() {
        pimpl->unlock_mouse_cursor();
    }
#endif

} // namespace PM3D
