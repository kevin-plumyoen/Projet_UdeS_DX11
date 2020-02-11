#ifndef DEF_WIN32_BACKEND_H
#define DEF_WIN32_BACKEND_H

#include "BackendTraits.h"
#include "Windows.h"

#include <utility>

#define MAX_LOADSTRING 100

namespace PM3D {

class CDispositifD3D11;
class CDIManipulateur;

class Win32Backend {
public:
    using dispositif_type = CDispositifD3D11;
    using inputs_type = CDIManipulateur;

    Win32Backend();

    // Copy is disabled
    Win32Backend(const Win32Backend&) = delete;
    Win32Backend& operator=(const Win32Backend&) = delete;

    void SetPlatformAppInstance(void* platform_data);
    void SetWindowsAppInstance(HINSTANCE hInstance);

    int Initialisations(inputs_type& inputs);
	bool Run();
	dispositif_type* CreationDispositif(int cdsMode);
	void BeginRenderScene(dispositif_type* dispositif);
	void EndRenderScene(dispositif_type* dispositif);

    void notify_size_changed(int new_width, int new_height);
    bool size_changed() const noexcept;
    std::pair<int, int> last_screen_size() const noexcept;
private:
    ATOM MyRegisterClass(HINSTANCE hInstance);
	bool InitAppInstance();
	int Show();

	// Fonctions "Callback" -- Doivent être statiques
	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

	HACCEL hAccelTable;						// handle Windows de la table des accélérateurs
	static HINSTANCE hAppInstance;			// handle Windows de l'instance actuelle de l'application
	HWND hMainWnd;							// handle Windows de la fenêtre principale
	TCHAR szWindowClass[MAX_LOADSTRING];	// le nom de la classe de fenêtre principale
    mutable bool size_changed_;
    int window_width, window_height;
};

template<>
struct is_backend<Win32Backend> {
    constexpr static const bool value = true; 
};

template<>
struct backend_traits<Win32Backend> {
    using dispositif_type = typename Win32Backend::dispositif_type;
    using inputs_type = typename Win32Backend::inputs_type;
};

}

#endif