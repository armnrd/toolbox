#ifndef TOOLBOX_WORKSPACES_HPP
#define TOOLBOX_WORKSPACES_HPP

#include <QApplication>
#include "config.hpp"

namespace toolbox::workspaces
{
    class Workspaces
    {
    public:
        Workspaces(QApplication *app, config::Config *config);
        ~Workspaces(); // save app lists to config on destruction

        void launch_base_apps();
        std::list<std::string> *get_base_app_list();
        void add_app_to_base_list(const std::string &app_name);
        void remove_app_from_base_list(const std::string &app_name);

        void launch_profile_apps(const std::string &profile_name);
        std::list<std::string> *get_profile_app_list(const std::string &profile_name);
        void add_app_to_profile_list(const std::string &profile_name, const std::string &app_name);
        void remove_app_from_profile_list(const std::string &profile_name, const std::string &app_name);
    };
} // namespace toolbox::workspaces

#endif // TOOLBOX_WORKSPACES_HPP
