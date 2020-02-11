#ifndef CONTROLLERLAYOUT_H
#define CONTROLLERLAYOUT_H

#include "KeyboardKeys.h"

struct ControllerLayout {
  using button_value = KeyboardKey;
  using axis_value = long;

  button_value up;
  button_value down;
  button_value right;
  button_value left;
  button_value jump;

  button_value alt_up;
  button_value alt_down;
  button_value alt_right;
  button_value alt_left;

  button_value change_camera;
  button_value pause;

  axis_value x_axis;
  axis_value y_axis;

private:
    ControllerLayout() = default;
public:
    static ControllerLayout& get() noexcept {
        static ControllerLayout instance;

        return instance;
    }
};

#endif