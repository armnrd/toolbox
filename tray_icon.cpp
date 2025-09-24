#include "tray_icon.hpp"
#include <QFile>
#include <QMenu>

namespace toolbox::tray_icon
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

    TrayIcon::~TrayIcon()
    {
        delete app;
    }

    void TrayIcon::show_config_editor()
    {
        config::show_editor(config);
        // use config::load_config()
    }

    AppTrayIcon::AppTrayIcon(QApplication *app, config::Config *config) : QSystemTrayIcon(app)
    {
    }

    AppTrayIcon::~AppTrayIcon()
    {
        delete app;
    }

    void AppTrayIcon::restore_app_window()
    {
    }
} // namespace toolbox::tray_icon
