#include "windowmgr.hpp"
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QIntValidator>
#include <QPushButton>
#include <QDebug>
#include <QLabel>
#include <windows.h>

// TODO beautify
// TODO file scope vs global scope for classes like this
class ResizeDialog : public QDialog
{
public:
    ResizeDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("Enter Window Dimensions");
        setModal(true);

        QVBoxLayout *main_layout = new QVBoxLayout(this);
        QHBoxLayout *width_layout = new QHBoxLayout();
        QHBoxLayout *height_layout = new QHBoxLayout();
        QHBoxLayout *button_layout = new QHBoxLayout();

        QLabel *width_label = new QLabel("Width:");
        width_input = new QLineEdit(this);
        width_input->setValidator(new QIntValidator(100, 2000, this));
        width_layout->addWidget(width_label);
        width_layout->addWidget(width_input);

        QLabel *height_label = new QLabel("Height:");
        height_input = new QLineEdit(this);
        height_input->setValidator(new QIntValidator(100, 2000, this));
        height_layout->addWidget(height_label);
        height_layout->addWidget(height_input);

        QPushButton *resize_button = new QPushButton("Resize", this);
        QPushButton *cancel_button = new QPushButton("Cancel", this);
        button_layout->addWidget(resize_button);
        button_layout->addWidget(cancel_button);

        main_layout->addLayout(width_layout);
        main_layout->addLayout(height_layout);
        main_layout->addLayout(button_layout);

        connect(resize_button, &QPushButton::clicked, this, &ResizeDialog::accept);
        connect(cancel_button, &QPushButton::clicked, this, &ResizeDialog::reject);
    }

    int input_width() const { return width_input->text().toInt(); }
    int input_height() const { return height_input->text().toInt(); }

private:
    QLineEdit *width_input;
    QLineEdit *height_input;
};

// Distance constants for the move functions
static const long FAST_DISTANCE = 50, SLOW_DISTANCE = 10;

static RECT window_rect_from_hwnd(HWND hwnd)
{
    // Get the dimensions of the window
    RECT rect;
    if (!GetWindowRect(hwnd, &rect))
    {
        qDebug() << "Error: Could not get window dimensions!";
        return {};
    }

    return rect;
}

static RECT screen_rect_from_hwnd(HWND hwnd)
{
    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO moninfo;
    moninfo.cbSize = sizeof(MONITORINFO);

    if (!GetMonitorInfo(hmon, &moninfo))
    {
        qDebug() << "Error: Could not get monitor information!";
        return {};
    }

    return moninfo.rcWork;
}

void restore_if_maximized(HWND hwnd)
{
    // Check if the window is maximized
    if (IsZoomed(hwnd))
    {
        // If it is maximized, restore it
        ShowWindow(hwnd, SW_RESTORE);
    }
}

namespace toolbox::windowmgr
{
    void minimize(HWND hwnd)
    {
        ShowWindow(hwnd, SW_MINIMIZE);
    }

    void toggle_maximize(HWND hwnd)
    {
        // Check if the window is maximized
        if (IsZoomed(hwnd))
        {
            ShowWindow(hwnd, SW_RESTORE);
        } else
        {
            ShowWindow(hwnd, SW_MAXIMIZE);
        }
    }

    void close(HWND hwnd)
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }

    void action_foreground_close()
    {
        close(GetForegroundWindow());
    }

    void resize(HWND hwnd)
    {
        // TODO this is a hack - beautify it
        ResizeDialog dialog;
        dialog.setProperty("_q_windowsParentHwnd", reinterpret_cast<WId>(hwnd));

        auto rect = window_rect_from_hwnd(hwnd);

        if (dialog.exec() == QDialog::Accepted)
        {
            if (!SetWindowPos(hwnd, nullptr, rect.left, rect.top, dialog.input_width(),
                              dialog.input_height(), SWP_NOZORDER))
            {
                qDebug() << "Error: Could not resize the window!";
            }
        }
    }

    void centre(HWND hwnd)
    {
        // Check if the window is maximized
        if (IsZoomed(hwnd))
        {
            // If it is maximized, do nothing
            return;
        }

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);
        auto scr_width = scr_rect.right - scr_rect.left;
        auto scr_height = scr_rect.bottom - scr_rect.top;

        // Calculate the target position
        long target_x = scr_rect.left + (scr_width - win_width) / 2;
        long target_y = scr_rect.top + (scr_height - win_height) / 2;

        // Move the window to the centre position
        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, win_width, win_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_lower_screen(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto scr_rect = screen_rect_from_hwnd(hwnd);
        auto scr_width = scr_rect.right - scr_rect.left;
        auto scr_height = scr_rect.bottom - scr_rect.top;

        // Calculate the target position
        long target_width = scr_width * 0.65;
        long target_height = scr_height * 0.75;
        long target_x = (scr_rect.right - target_width) / 2;
        long target_y = scr_rect.bottom - target_height;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_upper_screen(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto scr_rect = screen_rect_from_hwnd(hwnd);
        auto scr_width = scr_rect.right - scr_rect.left;
        auto scr_height = scr_rect.bottom - scr_rect.top;

        // Calculate the target position
        long target_width = scr_width * 0.65;
        long target_height = scr_height * 0.75;
        long target_x = (scr_rect.right - target_width) / 2;
        long target_y = 0;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_right_middle(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = scr_rect.right - target_width;
        long target_y = (scr_rect.bottom - target_height) / 2;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_top_right(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = scr_rect.right - target_width;
        long target_y = 0;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_top_middle(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = (scr_rect.right - target_width) / 2;
        long target_y = 0;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_top_left(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = 0;
        long target_y = 0;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_left_middle(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = 0;
        long target_y = (scr_rect.bottom - target_height) / 2;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_bottom_left(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = 0;
        long target_y = scr_rect.bottom - target_height;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_bottom_middle(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = (scr_rect.right - target_width) / 2;
        long target_y = scr_rect.bottom - target_height;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void position_bottom_right(HWND hwnd)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = scr_rect.right - target_width;
        long target_y = scr_rect.bottom - target_height;

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_left(HWND hwnd, long distance)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = win_rect.left - distance;
        long target_y = win_rect.top;

        if (target_x <= scr_rect.left)
        {
            // Window at the target edge; resize it
            target_width = win_width - distance;
            target_x = 0;
        }

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_right(HWND hwnd, long distance)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = win_rect.left + distance;
        long target_y = win_rect.top;

        if (target_x + target_width >= scr_rect.right)
        {
            // Window at the target edge; resize it
            target_width = win_width - distance;
            target_x = scr_rect.right - target_width;
        }

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_up(HWND hwnd, long distance)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = win_rect.left;
        long target_y = win_rect.top - distance;

        if (target_y <= scr_rect.top)
        {
            // Window at the target edge; resize it
            target_height = win_height - distance;
            target_y = 0;
        }

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void move_down(HWND hwnd, long distance)
    {
        restore_if_maximized(hwnd);

        auto win_rect = window_rect_from_hwnd(hwnd);
        auto win_width = win_rect.right - win_rect.left;
        auto win_height = win_rect.bottom - win_rect.top;
        auto scr_rect = screen_rect_from_hwnd(hwnd);

        // Calculate the target position
        long target_width = win_width;
        long target_height = win_height;
        long target_x = win_rect.left;
        long target_y = win_rect.top + distance;

        if (target_y + target_height >= scr_rect.bottom)
        {
            // Window at the target edge; resize it
            target_height = win_height - distance;
            target_y = scr_rect.bottom - target_height;
        }

        if (!SetWindowPos(hwnd, nullptr, target_x, target_y, target_width, target_height, SWP_NOZORDER))
        {
            qDebug() << "Error: Could not move the window!";
        }
    }

    void action_foreground_minimize()
    {
        minimize(GetForegroundWindow());
    }

    void action_foreground_toggle_maximize()
    {
        toggle_maximize(GetForegroundWindow());
    }

    void action_foreground_resize()
    {
        resize(GetForegroundWindow());
    }

    void position_foreground_centre()
    {
        centre(GetForegroundWindow());
    }

    void position_foreground_lower_screen()
    {
        position_lower_screen(GetForegroundWindow());
    }

    void position_foreground_upper_screen()
    {
        position_upper_screen(GetForegroundWindow());
    }

    void position_foreground_right_middle()
    {
        position_right_middle(GetForegroundWindow());
    }

    void position_foreground_top_right()
    {
        position_top_right(GetForegroundWindow());
    }

    void position_foreground_top_middle()
    {
        position_top_middle(GetForegroundWindow());
    }

    void position_foreground_top_left()
    {
        position_top_left(GetForegroundWindow());
    }

    void position_foreground_left_middle()
    {
        position_left_middle(GetForegroundWindow());
    }

    void position_foreground_bottom_left()
    {
        position_bottom_left(GetForegroundWindow());
    }

    void position_foreground_bottom_middle()
    {
        position_bottom_middle(GetForegroundWindow());
    }

    void position_foreground_bottom_right()
    {
        position_bottom_right(GetForegroundWindow());
    }

    void move_foreground_fast_left()
    {
        move_left(GetForegroundWindow(), FAST_DISTANCE);
    }

    void move_foreground_slow_left()
    {
        move_left(GetForegroundWindow(), SLOW_DISTANCE);
    }

    void move_foreground_fast_right()
    {
        move_right(GetForegroundWindow(), FAST_DISTANCE);
    }

    void move_foreground_slow_right()
    {
        move_right(GetForegroundWindow(), SLOW_DISTANCE);
    }

    void move_foreground_fast_up()
    {
        move_up(GetForegroundWindow(), FAST_DISTANCE);
    }

    void move_foreground_slow_up()
    {
        move_up(GetForegroundWindow(), SLOW_DISTANCE);
    }

    void move_foreground_fast_down()
    {
        move_down(GetForegroundWindow(), FAST_DISTANCE);
    }

    void move_foreground_slow_down()
    {
        move_down(GetForegroundWindow(), SLOW_DISTANCE);
    }
}
