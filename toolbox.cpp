#include "toolbox.hpp"

namespace toolbox
{
    Toolbox::Toolbox(int& argc, char** argv) : QApplication(argc, argv)
    {
        // Load configuration
        config = config::config_from_file("config.yaml");

        // Set up tray icon
        tray_icon = new tray_icon::TrayIcon(this, config);
        tray_icon->show();
        tray_icon->showMessage("Toolbox", "Running in the system tray.", QSystemTrayIcon::Information, 2000);

        // Set up hotkeys
        hotkeys = new hotkeys::Hotkeys(this, config);
        hotkeys->register_hotkeys();


        // Set up workspaces
        // workspaces = new workspaces::Workspaces(this, config);
        // workspaces->launch_base_apps();
        qDebug() << "Toolbox initialized: tray icon and hotkeys set up.";
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
