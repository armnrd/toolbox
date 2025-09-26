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

        // Set up keymap
        kmmgr = new keymap::KeymapMgr(this);
        kmmgr->add_keybinds_from_config(config);
        qDebug() << "Registered keymap from config.";

        // Set up workspaces
        wsmgr = new workspaces::WorkspaceMgr(config, kmmgr);
        qDebug() << "Set up workspaces and launched base apps.";

        wsmgr->launch_terminal();
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
