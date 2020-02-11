#pragma once

#include "MathHelpers.h"
#include "KeyboardKeys.h"
#include <memory>
#include <cstdint>

namespace PM3D
{

struct MouseState {
    long x_;
    long y_;
    long z_;

    bool buttons_[4];
public:
    MouseState(long x, long y, long z, bool l_button, bool r_button)
    : x_{ x }
    , y_{ y }
    , z_{ z } {
        buttons_[0] = l_button;
        buttons_[1] = r_button;
    }

    long x() const noexcept { return x_; }
    long y() const noexcept { return y_; }
    long z() const noexcept { return z_; }

    bool left_button_pressed() const noexcept { return buttons_[0]; }
    bool right_button_pressed() const noexcept { return buttons_[1]; }
};


class CDIManipulateur 
{
public:
    using init_data = const void*;

	CDIManipulateur();
	~CDIManipulateur();

	bool Init(init_data init);
	void StatutClavier();
	bool ToucheAppuyee(KeyboardKey touche) const;
	void SaisirEtatSouris();

    void set_window_handle(const void* window_handle);

	MouseState EtatSouris() const;
    Vec2 normalized_mouse_coords() const;

    void update_screen_size(int new_width, int new_height);

    void hide_cursor();
    void show_cursor();

#ifndef NEDITOR
    void unlock_cursor();
#endif

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

	static bool bDejaInit;
   
	char tamponClavier[256];
    int screen_width, screen_height;
	
};

} // namespace PM3D
