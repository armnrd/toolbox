#include "tray.hpp"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QMenu>
#include <QStyle>
#include <windows.h>
#include <psapi.h>

// Returns the executable path for a given HWND
static QString exe_path_from_hwnd(HWND hwnd)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess)
        return {};

    wchar_t exePath[MAX_PATH] = {};
    if (GetModuleFileNameExW(hProcess, nullptr, exePath, MAX_PATH))
    {
        CloseHandle(hProcess);
        return QString::fromWCharArray(exePath);
    }
    CloseHandle(hProcess);
    return {};
}

// Extracts the large icon from an executable path
static QIcon qicon_from_exe_path(const QString &exePath)
{
    HICON hIconLarge = nullptr;
    ExtractIconExW(reinterpret_cast<const wchar_t *>(exePath.utf16()), 0, &hIconLarge, nullptr, 1);
    if (!hIconLarge)
        return {};

    QPixmap pixmap = QPixmap::fromImage(QImage::fromHICON(hIconLarge));
    DestroyIcon(hIconLarge);
    return QIcon(pixmap);
}

namespace toolbox::tray
{
    TrayIcon::TrayIcon(QApplication *app, config::Config *config) : QSystemTrayIcon(app)
    {
        // TODO error handling
        // Check if the system tray is available
        if (!QSystemTrayIcon::isSystemTrayAvailable())
        {
            qDebug() << "Error: System tray not available on this system.";
            return;
        }

        // Store reference to Toolbox and Config objects for later
        this->app = app;
        this->config = config;

        // Set the icon
        setIcon(QIcon(":/toolbox.ico"));

        // Create a context menu for the tray icon
        auto menu = new QMenu();

        // Add actions to the menu
        QAction *menu_settings = new QAction("Settings...", menu);
        QAction *menu_quit = new QAction("Quit", menu);

        menu->addAction(menu_settings);
        menu->addSeparator();
        menu->addAction(menu_quit);

        // Connect actions to slots
        connect(menu_settings, &QAction::triggered, this, &TrayIcon::show_config_editor);
        connect(menu_quit, &QAction::triggered, this, &QApplication::quit);

        // Set the context menu for the tray icon
        setContextMenu(menu);
    }

    void TrayIcon::show_config_editor()
    {
        config::show_editor(config);
        // use config::load_config()
    }

    AppTrayIcon::AppTrayIcon(HWND app_hwnd, QIcon app_icon) : QSystemTrayIcon(nullptr)
    {
        this->app_hwnd = app_hwnd;
        this->app_icon = app_icon;

        // TODO error handling
        // Check if the system tray is available
        if (!QSystemTrayIcon::isSystemTrayAvailable())
        {
            qDebug() << "Error: System tray not available on this system.";
            return;
        }

        // Set the icon
        setIcon(app_icon);

        // Create a context menu for the tray icon
        auto menu = new QMenu();

        // Add actions to the menu
        QAction *menu_restore = new QAction("Restore", menu);

        menu->addAction(menu_restore);

        // Connect actions to slots
        connect(menu_restore, &QAction::triggered, this, &AppTrayIcon::unhide_app_window);

        // Set the context menu for the tray icon
        setContextMenu(menu);
    }

    void AppTrayIcon::toggle_app_window()
    {
        if (IsWindowVisible(app_hwnd))
        {
            hide_app_window();
        } else
        {
            unhide_app_window();
        }
    }

    void AppTrayIcon::hide_app_window()
    {
        ShowWindow(app_hwnd, SW_HIDE);
        show(); // Show tray icon
        setToolTip("App is hidden. Click to restore.");
    }

    void AppTrayIcon::unhide_app_window()
    {
        ShowWindow(app_hwnd, SW_SHOW);
        hide(); // Hide tray icon
    }

    AppTrayIcon *create_app_tray_icon(HWND window_handle)
    {
        // Get app icon from window handle
        auto app_path = exe_path_from_hwnd(window_handle);
        QIcon app_icon;
        if (app_path.isEmpty())
        {
            app_icon = QApplication::style()->standardIcon(QStyle::SP_DesktopIcon);
        } else
        {
            app_icon = qicon_from_exe_path(exe_path_from_hwnd(window_handle));
        }

        return new AppTrayIcon(window_handle, app_icon);
    }
}
