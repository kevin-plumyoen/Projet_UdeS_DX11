#include "resource.h"

#include "Win32Backend.h"
#include "DInputInitData.h"
#include "dispositifD3D11.h"
#include "DIManipulateur.h"

#include <algorithm>
#include <iterator>

namespace PM3D
{
    HINSTANCE Win32Backend::hAppInstance = NULL;

    Win32Backend::Win32Backend() 
    : hAccelTable{NULL}
    , hMainWnd{NULL} 
    , size_changed_{false}
    , window_width{0}
    , window_height{0} {
        std::fill(std::begin(szWindowClass), std::end(szWindowClass), static_cast<TCHAR>(0));
    }

    void Win32Backend::SetPlatformAppInstance(void* platform_data) {
        HINSTANCE* instance = reinterpret_cast<HINSTANCE*>(platform_data);
        SetWindowsAppInstance(*instance);
    }

    void Win32Backend::SetWindowsAppInstance(HINSTANCE hInstance)
    {
        hAppInstance = hInstance; // Stocke le handle d'instance de l'application, plusieurs fonctions spécifiques en auront besoin
    }

    ATOM Win32Backend::MyRegisterClass(HINSTANCE hInstance)
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = &WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PETITMOTEUR3D));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        return RegisterClassEx(&wcex);
    }

    bool Win32Backend::InitAppInstance()
    {
        TCHAR szTitle[MAX_LOADSTRING];					// Le texte de la barre de titre

        // Initialise les chaînes globales
        LoadString(hAppInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadString(hAppInstance, IDC_PETITMOTEUR3D, szWindowClass, MAX_LOADSTRING);
        MyRegisterClass(hAppInstance);

        hMainWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hAppInstance, this);

        if (!hMainWnd)
        {
            return false;
        }

        hAccelTable = LoadAccelerators(hAppInstance, MAKEINTRESOURCE(IDC_PETITMOTEUR3D));

        return true;
    }

    int Win32Backend::Show()
    {
        ShowWindow(hMainWnd, SW_SHOWNORMAL);
        UpdateWindow(hMainWnd);

        return 0;
    }


    int Win32Backend::Initialisations(inputs_type& inputs) {
        DInputInitData input_init_data;
        input_init_data.hInstance = hAppInstance;
        input_init_data.hWnd = hMainWnd;
        inputs.Init(&input_init_data);

        InitAppInstance();
        Show();

        inputs.set_window_handle(&hMainWnd);

        return 0;
    }

    bool Win32Backend::Run()
    {
        MSG msg;
        bool bBoucle = true;

        // Y-a-t'il un message Windows à traiter?
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Est-ce un message de fermeture ?
            if (msg.message == WM_QUIT) bBoucle = false;

            // distribuer le message
            if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }

        return bBoucle;
    }

    Win32Backend::dispositif_type* Win32Backend::CreationDispositif(int cdsMode)
    {
        return new CDispositifD3D11(static_cast<CDS_MODE>(cdsMode), hMainWnd);
    }

    void Win32Backend::BeginRenderScene(dispositif_type* /*pDispositif*/)
    {

    }

    void Win32Backend::EndRenderScene(dispositif_type*)
    {
    }

    void Win32Backend::notify_size_changed(int w, int h) {
        size_changed_ = true;

        window_width = w;
        window_height = h;
    }

    bool Win32Backend::size_changed() const noexcept {
        return size_changed_;
    }

    std::pair<int, int> Win32Backend::last_screen_size() const noexcept {
        size_changed_ = false;

        return std::make_pair(window_width, window_height);
    }

    LRESULT CALLBACK Win32Backend::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        int wmId, wmEvent;
        PAINTSTRUCT ps;
        HDC hdc;

        Win32Backend* backend = reinterpret_cast<Win32Backend*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        switch (message)
        {
        case WM_NCCREATE:
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams));
            return TRUE;
        case WM_COMMAND:
            wmId = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            // Analyse les sélections de menu :
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hAppInstance, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            // Évitez d'ajouter du code ici...
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE: {
            const int new_width = LOWORD(lParam);
            const int new_height = HIWORD(lParam);
            backend->notify_size_changed(new_width, new_height);
        }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    // Gestionnaire de messages pour la boîte de dialogue À propos de.
    INT_PTR CALLBACK Win32Backend::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(lParam);
        switch (message)
        {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
        }
        return (INT_PTR)FALSE;
    }

}
