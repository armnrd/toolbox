#include "toolbox.hpp"

namespace toolbox
{
    Toolbox::Toolbox(int& argc, char** argv) : QApplication(argc, argv)
    {
        // Load configuration
        config = config::config_from_file("config.yaml");

        // Set up tray icon
        tray_icon = new tray::TrayIcon(this, config);
        tray_icon->show();
        tray_icon->showMessage("Toolbox", "Running in the system tray.", QSystemTrayIcon::Information, 2000);
        qDebug() << "Created tray icon.";

        // Set up hotkeys
        hotkeys = new hotkeys::Hotkeys(this);
        hotkeys->add_hotkeys_from_config(config);
        qDebug() << "Registered hotkeys from config.";

        // Set up workspaces
        workspaces = new workspaces::Workspaces(this, config);
        // workspaces->launch_base_apps();
        // qDebug() << "Set up workspaces and launched base apps.";
    };

    Toolbox::~Toolbox() = default;
}

int main(int argc, char* argv[])
{
    try
    {
        const auto app = new toolbox::Toolbox(argc, argv);
        return app->exec();
    }
    catch (const std::exception& e)
    {
        qDebug() << "Caught exception: " << e.what() << '\n';
    }
}
