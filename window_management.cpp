#include "window_management.hpp"
#include <QDebug>
#include <windows.h>

// TODO beautify
// TODO file scope vs global scope for classes like this
// class ResizeDialog : public QDialog {
// public:
//     ResizeDialog(QWidget *parent = nullptr) : QDialog(parent) {
//         setWindowTitle("Enter Window Dimensions");
//         setModal(true);
//
//         QVBoxLayout *main_layout = new QVBoxLayout(this);
//         QHBoxLayout *width_layout = new QHBoxLayout();
//         QHBoxLayout *height_layout = new QHBoxLayout();
//         QHBoxLayout *button_layout = new QHBoxLayout();
//
//         QLabel *width_label = new QLabel("Width:");
//         width_input = new QLineEdit(this);
//         width_input->setValidator(new QIntValidator(100, 2000, this));
//         width_layout->addWidget(width_label);
//         width_layout->addWidget(width_input);
//
//         QLabel *height_label = new QLabel("Height:");
//         height_input = new QLineEdit(this);
//         height_input->setValidator(new QIntValidator(100, 2000, this));
//         height_layout->addWidget(height_label);
//         height_layout->addWidget(height_input);
//
//         QPushButton *resize_button = new QPushButton("Resize", this);
//         QPushButton *cancel_button = new QPushButton("Cancel", this);
//         button_layout->addWidget(resize_button);
//         button_layout->addWidget(cancel_button);
//
//         main_layout->addLayout(width_layout);
//         main_layout->addLayout(height_layout);
//         main_layout->addLayout(button_layout);
//
//         connect(resize_button, &QPushButton::clicked, this, &ResizeDialog::accept);
//         connect(cancel_button, &QPushButton::clicked, this, &ResizeDialog::reject);
//     }
//
//     int input_width() const { return width_input->text().toInt(); }
//     int input_height() const { return height_input->text().toInt(); }
//
// private:
//     QLineEdit *width_input;
//     QLineEdit *height_input;
// };

// Distance constants for the move functions
static const long FAST_DISTANCE = 50, SLOW_DISTANCE = 10;

// TODO error handling
/**
 * @brief Returns the bounds of the currently focused window.
 *
 * @return {window_left, window_right, window_top, window_bottom, window_width, window_height}
 * @note
 */
static std::tuple<HWND, RECT, long, long> foreground_window_info()
{
    // Get the handle to the currently focused window
    HWND window_handle = GetForegroundWindow();
    if (!window_handle)
    {
        qDebug() << "Error: No foreground window found!";
        return {};
    }

    // Get the dimensions of the window
    RECT window_rect;
    if (!GetWindowRect(window_handle, &window_rect))
    {
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
static std::tuple<HMONITOR, RECT, long, long> get_screen_info(HWND window_handle)
{
    // Get the screen dimensions of the monitor_handle where the window is located
    HMONITOR monitor_handle = MonitorFromWindow(window_handle, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitor_info;
    monitor_info.cbSize = sizeof(MONITORINFO);

    if (!GetMonitorInfo(monitor_handle, &monitor_info))
    {
        qDebug() << "Error: Could not get monitor_handle information!";
        return {};
    }

    RECT screen_rect = monitor_info.rcWork;
    int screen_width = screen_rect.right - screen_rect.left;
    int screen_height = screen_rect.bottom - screen_rect.top;

    return {monitor_handle, screen_rect, screen_width, screen_height};
}

namespace toolbox::window_management
{
    void action_minimize()
    {
        // Obtain window info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();

        // Minimize it
        ShowWindow(window_handle, SW_MINIMIZE);
    }

    void action_toggle_maximize()
    {
        // Obtain window info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        } else
        {
            // If it is not maximized, maximize it
            ShowWindow(window_handle, SW_MAXIMIZE);
        }
    }

    void action_close()
    {
        // Obtain window info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();

        // Close it
        PostMessage(window_handle, WM_CLOSE, 0, 0);
    }

    void action_resize()
    {
        // Obtain window info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();

        // TODO hack - remove when beautifying dialog
        // ResizeDialog dialog;
        // dialog.setProperty("_q_windowsParentHwnd", reinterpret_cast<WId>(window_handle));

        // Resize it
        // if (dialog.exec() == QDialog::Accepted) {
        //     if (!SetWindowPos(window_handle, nullptr, window_rect.left, window_rect.top, dialog.input_width(),
        //                       dialog.input_height(), SWP_NOZORDER)) {
        //         qDebug() << "Error: Could not resize the window!";
        //     }
        // }
    }

    void position_centre()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, do nothing
            return;
        }

        // Calculate the target position
        long target_x = screen_rect.left + (screen_width - window_width) / 2;
        long target_y = screen_rect.top + (screen_height - window_height) / 2;

        // Move the window to the center position
        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, window_width, window_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_lower_screen()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = screen_width * 0.65;
        long target_height = screen_height * 0.75;
        long target_x = (screen_rect.right - target_width) / 2;
        long target_y = screen_rect.bottom - target_height;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_upper_screen()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = screen_width * 0.65;
        long target_height = screen_height * 0.75;
        long target_x = (screen_rect.right - target_width) / 2;
        long target_y = 0;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_right_middle()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = screen_rect.right - target_width;
        long target_y = (screen_rect.bottom - target_height) / 2;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_top_right()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = screen_rect.right - target_width;
        long target_y = 0;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_top_middle()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = (screen_rect.right - target_width) / 2;
        long target_y = 0;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_top_left()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = 0;
        long target_y = 0;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_left_middle()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = 0;
        long target_y = (screen_rect.bottom - target_height) / 2;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_bottom_left()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = 0;
        long target_y = screen_rect.bottom - target_height;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_bottom_middle()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = (screen_rect.right - target_width) / 2;
        long target_y = screen_rect.bottom - target_height;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_bottom_right()
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = screen_rect.right - target_width;
        long target_y = screen_rect.bottom - target_height;

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_left(long distance)
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = window_rect.left - distance;
        long target_y = window_rect.top;

        if (target_x <= screen_rect.left)
        {
            // Window at the target edge; resize it
            target_width = window_width - distance;
            target_x = 0;
        }

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_right(long distance)
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = window_rect.left + distance;
        long target_y = window_rect.top;

        if (target_x + target_width >= screen_rect.right)
        {
            // Window at the target edge; resize it
            target_width = window_width - distance;
            target_x = screen_rect.right - target_width;
        }

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_up(long distance)
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = window_rect.left;
        long target_y = window_rect.top - distance;

        if (target_y <= screen_rect.top)
        {
            // Window at the target edge; resize it
            target_height = window_height - distance;
            target_y = 0;
        }

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_down(long distance)
    {
        // Obtain window and screen info
        auto [window_handle, window_rect, window_width, window_height] = foreground_window_info();
        auto [monitor_handle, screen_rect, screen_width, screen_height] = get_screen_info(window_handle);

        // Check if the window is maximized
        if (IsZoomed(window_handle))
        {
            // If it is maximized, restore it
            ShowWindow(window_handle, SW_RESTORE);
        }

        // Calculate the target position
        long target_width = window_width;
        long target_height = window_height;
        long target_x = window_rect.left;
        long target_y = window_rect.top + distance;

        if (target_y + target_height >= screen_rect.bottom)
        {
            // Window at the target edge; resize it
            target_height = window_height - distance;
            target_y = screen_rect.bottom - target_height;
        }

        if (!SetWindowPos(window_handle, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_fast_left()
    {
        move_left(FAST_DISTANCE);
    }

    void move_slow_left()
    {
        move_left(SLOW_DISTANCE);
    }

    void move_fast_right()
    {
        move_right(FAST_DISTANCE);
    }

    void move_slow_right()
    {
        move_right(SLOW_DISTANCE);
    }

    void move_fast_up()
    {
        move_up(FAST_DISTANCE);
    }

    void move_slow_up()
    {
        move_up(SLOW_DISTANCE);
    }

    void move_fast_down()
    {
        move_down(FAST_DISTANCE);
    }

    void move_slow_down()
    {
        move_down(SLOW_DISTANCE);
    }
} // namespace toolbox::window_management
