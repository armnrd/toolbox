#ifndef TOOLBOX_TOOLBOX_HPP
#define TOOLBOX_TOOLBOX_HPP

#include "config.hpp"
#include "tray_icon.hpp"
#include "hotkeys.hpp"
// #include "workspaces.hpp"
#include <QApplication>


namespace toolbox
{
    // Just holds information that needs to be passed around the various parts.
    class Toolbox : public QApplication
    {
    public:
        Toolbox(int &argc, char **argv);
        ~Toolbox() override;
        
    private:
        config::Config *config;
        tray_icon::TrayIcon *tray_icon;
        hotkeys::Hotkeys *hotkeys;
        // workspaces::Workspaces *workspaces;
    };
}

#endif //TOOLBOX_TOOLBOX_HPP
