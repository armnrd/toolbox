#ifndef TOOLBOX_WORKSPACES_HPP
#define TOOLBOX_WORKSPACES_HPP

#include "config.hpp"
#include "keymap.hpp"
#include <QApplication>
#include <windows.h>

using std::string;
using std::wstring;
using std::map;

struct AppConfig
{
    wstring location; // wstring necessary because of CreateProcessW()
    wstring executable;
    wstring args;
    string position;
    string toggle_window;
    wstring window_identifier;
};

namespace YAML
{
    template<>
    struct convert<AppConfig>
    {
        static Node encode(const AppConfig &ac)
        {
            // TODO: handle null values
            Node node;
            node["location"] = string(ac.location.begin(), ac.location.end());
            node["executable"] = string(ac.executable.begin(), ac.executable.end());
            node["args"] = string(ac.args.begin(), ac.args.end());
            node["position"] = string(ac.position.begin(), ac.position.end());
            node["toggle window"] = string(ac.toggle_window.begin(), ac.toggle_window.end());
            node["window_identifier"] = string(ac.window_identifier.begin(), ac.window_identifier.end());
            return node;
        }

        static bool decode(const Node &node, AppConfig &ac)
        {
            if (!node.IsMap())
            {
                return false;
            }

            // TODO: nullptr when empty?
            auto location_str = node["location"].as<string>();
            auto executable_str = node["executable"].as<string>();
            auto args_str = node["args"].as<string>();
            auto position_str = node["position"].as<string>();
            auto toggle_window_str = node["toggle window"].as<string>();
            auto window_identifier_str = node["window identifier"].as<string>();

            // If args_str is "null", make it an empty string to avoid problems with CreateProcessW()
            if (args_str == "null")
            {
                args_str = "";
            }

            ac.location = wstring(location_str.begin(), location_str.end());
            ac.executable = wstring(executable_str.begin(), executable_str.end());
            ac.args = wstring(args_str.begin(), args_str.end());
            ac.position = position_str;
            ac.toggle_window = toggle_window_str;
            ac.window_identifier = wstring(window_identifier_str.begin(), window_identifier_str.end());

            qDebug() << format(L"Decoded AppConfig: location: '{}', executable: '{}', args: '{}'",
                               ac.location, ac.executable, ac.args);
            // TODO is there an automatic linebreak?
            qDebug() << format(", position: '{}', toggle window: '{}'", ac.position, ac.toggle_window);

            return true;
        }
    };
} // namespace YAML

namespace toolbox::workspaces
{
    void check_terminal();

    class WorkspaceMgr
    {
    public:
        void launch_terminal();

        WorkspaceMgr(config::Config *config, keymap::KeymapMgr *kmmgr);

        ~WorkspaceMgr();

        void launch_base_apps();

        std::list<std::string> *get_base_app_list();

        void add_app_to_base_list(const std::string &app_name);

        void remove_app_from_base_list(const std::string &app_name);

        void launch_profile_apps(const std::string &profile_name);

        std::list<std::string> *get_profile_app_list(const std::string &profile_name);

        void add_app_to_profile_list(const std::string &profile_name, const std::string &app_name);

        void remove_app_from_profile_list(const std::string &profile_name, const std::string &app_name);

    private:
        void launch_app(const AppConfig &ac);

        map<string, map<string, AppConfig> > workspaces;

        keymap::KeymapMgr *kmmgr;
    };
}

#endif // TOOLBOX_WORKSPACES_HPP
