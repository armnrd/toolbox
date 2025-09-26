#ifndef TOOLBOX_TOOLBOX_HPP
#define TOOLBOX_TOOLBOX_HPP

#include "config.hpp"
#include "keymap.hpp"
#include "tray.hpp"
#include "workspaces.hpp"
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
        tray::TrayIcon *tray_icon;
        keymap::KeymapMgr *kmmgr;
        workspaces::WorkspaceMgr *wsmgr;
    };
}

#endif //TOOLBOX_TOOLBOX_HPP
