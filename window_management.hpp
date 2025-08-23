#ifndef TOOLBOX_WINDOW_MANAGEMENT_HPP
#define TOOLBOX_WINDOW_MANAGEMENT_HPP

#include "config.hpp"
#include <string>

namespace window_management
{
    void action_minimize();

    void action_toggle_maximize();

    void action_close();

    void action_resize();

    void position_centre();

    void position_lower_screen();

    void position_upper_screen();

    void position_right_middle();

    void position_top_right();

    void position_top_middle();

    void position_top_left();

    void position_left_middle();

    void position_bottom_left();

    void position_bottom_middle();

    void position_bottom_right();

    void move_fast_left();

    void move_slow_left();

    void move_fast_right();

    void move_slow_right();

    void move_fast_up();

    void move_slow_up();

    void move_fast_down();

    void move_slow_down();
};

#endif //TOOLBOX_WINDOW_MANAGEMENT_HPP
