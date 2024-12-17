/**
 * @file window_management.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 16/12/2024
 */

#include "window_management.hpp"
#include <windows.h>
#include <QDebug>


// Distance constants for the move functions
static const long fast_distance = 50, slow_distance = 10;

// TODO error handling
/**
 * @brief Returns the bounds of the currently focused window.
 *
 * @return {window_left, window_right, window_top, window_bottom, window_width, window_height}
 * @note
 */
std::tuple<HWND, RECT, long, long> foreground_window_info()
{
    // Get the handle to the currently focused window
    HWND window_handle = GetForegroundWindow();
    if (!window_handle) {
        qDebug() << "Error: No foreground window found!";
        return {};
    }

    // Get the dimensions of the window
    RECT window_rect;
    if (!GetWindowRect(window_handle, &window_rect)) {
        qDebug() << "Error: Could not get window dimensions!";
        return {};
    }

    auto window_width = window_rect.right - window_rect.left;
    auto window_height = window_rect.bottom - window_rect.top;

    return {window_handle, window_rect, window_width, window_height};
}

// TODO error handling
/**
 * @brief Returns the bounds of the screen containing the window specified by window_handle.
 *
 * @param window_handle - handle of a window on the screen to find bounds for
 * @return {monitor_handle, screen_rect, screen_width, screen_height}
 * @note
 */
std::tuple<HMONITOR, RECT, long, long> screen_info(HWND window_handle)
{
    // Get the screen dimensions of the monitor_handle where the window is located
    HMONITOR monitor_handle = MonitorFromWindow(window_handle, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitor_info;
    monitor_info.cbSize = sizeof(MONITORINFO);

    if (!GetMonitorInfo(monitor_handle, &monitor_info)) {
        qDebug() << "Error: Could not get monitor_handle information!";
        return {};
    }

    RECT screen_rect = monitor_info.rcWork;
    int screen_width = screen_rect.right - screen_rect.left;
    int screen_height = screen_rect.bottom - screen_rect.top;

    return {monitor_handle, screen_rect, screen_width, screen_height};
}

void window_management::action_minimize()
{
    // Obtain window info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();

    // Minimize it
    ShowWindow(window_handle, SW_MINIMIZE);
}

void window_management::action_toggle_maximize()
{
    // Obtain window info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    } else {
        // If it is not maximized, maximize it
        ShowWindow(window_handle, SW_MAXIMIZE);
    }
}

void window_management::action_close()
{
    // Obtain window info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();

    // Close it
    PostMessage(window_handle, WM_CLOSE, 0, 0);
}

void window_management::action_resize()
{

}

void window_management::position_centre()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, do nothing
        return;
    }

    // Calculate the target position
    long target_x = screen_rect.left + (screen_width - window_width) / 2;
    long target_y = screen_rect.top + (screen_height - window_height) / 2;

    // Move the window to the center position
    if (!MoveWindow(window_handle, target_x, target_y, window_width, window_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_lower_screen()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = screen_width * 0.65;
    long target_height = screen_height * 0.75;
    long target_x = (screen_rect.right - target_width) / 2;
    long target_y = screen_rect.bottom - target_height;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_upper_screen()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = screen_width * 0.65;
    long target_height = screen_height * 0.75;
    long target_x = (screen_rect.right - target_width) / 2;
    long target_y = 0;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_right_middle()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = screen_rect.right - target_width;
    long target_y = (screen_rect.bottom - target_height) / 2;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_top_right()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = screen_rect.right - target_width;
    long target_y = 0;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_top_middle()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = (screen_rect.right - target_width) / 2;
    long target_y = 0;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_top_left()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = 0;
    long target_y = 0;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_left_middle()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = 0;
    long target_y = (screen_rect.bottom - target_height) / 2;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_bottom_left()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = 0;
    long target_y = screen_rect.bottom - target_height;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_bottom_middle()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = (screen_rect.right - target_width) / 2;
    long target_y = screen_rect.bottom - target_height;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void window_management::position_bottom_right()
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = screen_rect.right - target_width;
    long target_y = screen_rect.bottom - target_height;

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void move_left(long distance)
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = window_rect.left - distance;
    long target_y = window_rect.top;

    if (target_x <= screen_rect.left) {
        // Window at the target edge; resize it
        target_width = window_width - distance;
        target_x = 0;
    }

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void move_right(long distance)
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = window_rect.left + distance;
    long target_y = window_rect.top;

    if (target_x + target_width >= screen_rect.right) {
        // Window at the target edge; resize it
        target_width = window_width - distance;
        target_x = screen_rect.right - target_width;
    }

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }
}

void move_up(long distance)
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = window_rect.left;
    long target_y = window_rect.top - distance;

    if (target_y <= screen_rect.top) {
        // Window at the target edge; resize it
        target_height = window_height - distance;
        target_y = 0;
    }

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }

}

void move_down(long distance)
{
    // Obtain window and screen info
    auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
    auto [monitor_handle, screen_rect, screen_width, screen_height] = screen_info(window_handle);

    // Check if the window is maximized
    if (IsZoomed(window_handle)) {
        // If it is maximized, restore it
        ShowWindow(window_handle, SW_RESTORE);
    }

    // Calculate the target position
    long target_width = window_width;
    long target_height = window_height;
    long target_x = window_rect.left;
    long target_y = window_rect.top + distance;

    if (target_y + target_height >= screen_rect.bottom) {
        // Window at the target edge; resize it
        target_height = window_height - distance;
        target_y = screen_rect.bottom - target_height;
    }

    if (!MoveWindow(window_handle, target_x, target_y, target_width, target_height, TRUE)) {
        qDebug() << "Error: Could not move the window!";
    }

}

void window_management::move_fast_left()
{
    move_left(fast_distance);
}

void window_management::move_slow_left()
{
    move_left(slow_distance);
}

void window_management::move_fast_right()
{
    move_right(fast_distance);
}

void window_management::move_slow_right()
{
    move_right(slow_distance);
}

void window_management::move_fast_up()
{
    move_up(fast_distance);
}

void window_management::move_slow_up()
{
    move_up(slow_distance);
}

void window_management::move_fast_down()
{
    move_down(fast_distance);
}

void window_management::move_slow_down()
{
    move_down(slow_distance);
}
