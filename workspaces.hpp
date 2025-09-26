#ifndef TOOLBOX_WORKSPACES_HPP
#define TOOLBOX_WORKSPACES_HPP

#include "config.hpp"
#include <QApplication>
#include <windows.h>

namespace toolbox::workspaces
{
    void launch_terminal();

    class WorkspaceMgr
    {
    public:
        WorkspaceMgr(QApplication *app, config::Config *config);

        ~WorkspaceMgr();

        void launch_base_apps();

        std::list<std::string> *get_base_app_list();

        void add_app_to_base_list(const std::string &app_name);

        void remove_app_from_base_list(const std::string &app_name);

        void launch_profile_apps(const std::string &profile_name);

        std::list<std::string> *get_profile_app_list(const std::string &profile_name);

        void add_app_to_profile_list(const std::string &profile_name, const std::string &app_name);

        void remove_app_from_profile_list(const std::string &profile_name, const std::string &app_name);
    };
}

#endif // TOOLBOX_WORKSPACES_HPP
