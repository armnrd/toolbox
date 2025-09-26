#ifndef TOOLBOX_WINDOW_MANAGEMENT_HPP
#define TOOLBOX_WINDOW_MANAGEMENT_HPP

#include <windows.h>

namespace toolbox::windowmgr
{
    void action_foreground_minimize();

    void action_foreground_toggle_maximize();

    void action_foreground_close();

    void action_foreground_resize();

    void position_foreground_centre();

    void position_foreground_lower_screen();

    void position_foreground_upper_screen();

    void position_foreground_right_middle();

    void position_foreground_top_right();

    void position_foreground_top_middle();

    void position_foreground_top_left();

    void position_foreground_left_middle();

    void position_foreground_bottom_left();

    void position_foreground_bottom_middle();

    void position_foreground_bottom_right();

    void move_foreground_fast_left();

    void move_foreground_slow_left();

    void move_foreground_fast_right();

    void move_foreground_slow_right();

    void move_foreground_fast_up();

    void move_foreground_slow_up();

    void move_foreground_fast_down();

    void move_foreground_slow_down();

    void minimize(HWND hwnd);

    void toggle_maximize(HWND hwnd);

    void close(HWND hwnd);

    void resize(HWND hwnd);

    void centre(HWND hwnd);

    void position_lower_screen(HWND hwnd);

    void position_upper_screen(HWND hwnd);

    void position_right_middle(HWND hwnd);

    void position_top_right(HWND hwnd);

    void position_top_middle(HWND hwnd);

    void position_top_left(HWND hwnd);

    void position_left_middle(HWND hwnd);

    void position_bottom_left(HWND hwnd);

    void position_bottom_middle(HWND hwnd);

    void position_bottom_right(HWND hwnd);

    void move_left(HWND hwnd, long distance);

    void move_right(HWND hwnd, long distance);

    void move_up(HWND hwnd, long distance);

    void move_down(HWND hwnd, long distance);
}; // namespace toolbox::windowmgr

#endif // TOOLBOX_WINDOW_MANAGEMENT_HPP
