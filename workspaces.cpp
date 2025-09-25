#include "workspaces.hpp"

#include <QLabel>
#include <tlhelp32.h>
#include <windows.h>

// #include "workspaces_dashboard.h"
// #include "workspaces_dashboard_card.h"

using std::format;
using std::map;
using std::string;
using std::wstring;

struct App
{
    wstring location;
    wstring executable;
    wstring args;
    wstring position;
    wstring hide;
};

static map<string, map<string, App>> spaces = {};
// static toolbox::workspaces::WorkspacesDashboard *dashboard = nullptr;

namespace YAML
{
    template<>
    struct convert<App>
    {
        static Node encode(const App &app)
        {
            Node node;
            node["location"] = string(app.location.begin(), app.location.end());
            node["executable"] = string(app.executable.begin(), app.executable.end());
            node["args"] = string(app.args.begin(), app.args.end());
            node["position"] = string(app.position.begin(), app.position.end());
            node["hide"] = string(app.hide.begin(), app.hide.end());
            return node;
        }

        static bool decode(const Node &node, App &app)
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
            auto hide_str = node["hide"].as<string>();

            app.location = node["location"].IsNull() ? L"" : wstring(location_str.begin(), location_str.end());
            app.executable = node["executable"].IsNull() ? L"" : wstring(executable_str.begin(), executable_str.end());
            app.args = node["args"].IsNull() ? L"" : wstring(args_str.begin(), args_str.end());
            app.position = node["position"].IsNull() ? L"" : wstring(position_str.begin(), position_str.end());
            app.hide = node["hide"].IsNull() ? L"" : wstring(hide_str.begin(), hide_str.end());

            qDebug() << format(L"Decoded App: location='{}', executable='{}', args='{}', position='{}', hide='{}'",
                               app.location, app.executable, app.args, app.position, app.hide);

            return true;
        }
    };
} // namespace YAML

static std::list<string> get_running_processes()
{
    return {};
}

static bool is_process_running(const wstring &process_exe)
{
    HANDLE h_th_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (h_th_snap == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(h_th_snap, &pe32))
    {
        CloseHandle(h_th_snap);
        return false;
    }

    do
    {
        if (process_exe == pe32.szExeFile)
        {
            CloseHandle(h_th_snap);
            return true;
        }
    } while (Process32NextW(h_th_snap, &pe32));

    CloseHandle(h_th_snap);
    return false;
}

static void launch_process(const wstring &location, const wstring &executable, const wstring &args)
{
    qDebug() << format(L"Launching process: {}", executable);
    auto path = location + L"\\" + executable;
    auto command_line = path + L" " + args;
    STARTUPINFOW si = {sizeof(STARTUPINFOW)};
    PROCESS_INFORMATION pi;
    if (!CreateProcessW(path.data(), command_line.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, location.data(),
                        &si, &pi))
    {
        qDebug() << format(L"Failed to launch process: {}; error: {}", executable, GetLastError());
        // TODO: handle failure
        return;
    }
    qDebug() << format(L"Process launched: {}; pid: {}", executable, pi.dwProcessId);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

namespace toolbox::workspaces
{
    Workspaces::Workspaces(QApplication *app, config::Config *config)
    {
        auto base_workspace = map<string, App>{};
        for (auto it = (*config)["workspaces"]["base"].begin(); it != (*config)["workspaces"]["base"].end(); ++it)
        {
            auto base_app = it->second.as<App>();
            base_workspace[it->first.as<string>()] = base_app;
        }
        spaces["base"] = base_workspace;
    }

    Workspaces::~Workspaces()
    {
    }

    void Workspaces::launch_base_apps()
    {
        for (auto pair: spaces["base"])
        {
            auto app = pair.second;
            if (!is_process_running(app.executable))
            {
                launch_process(app.location, app.executable, app.args);
            } else
            {
                qDebug() << format(L"Process already running: {}", app.executable);
            }
        }
    }

    std::list<string> *Workspaces::get_base_app_list()
    {
        return nullptr;
    }

    void Workspaces::add_app_to_base_list(const string &app_name)
    {
    }

    void Workspaces::remove_app_from_base_list(const string &app_name)
    {
    }

    void Workspaces::launch_profile_apps(const string &profile_name)
    {
    }

    std::list<string> *Workspaces::get_profile_app_list(const string &profile_name)
    {
        return nullptr;
    }

    void Workspaces::add_app_to_profile_list(const string &profile_name, const string &app_name)
    {
    }

    void Workspaces::remove_app_from_profile_list(const string &profile_name, const string &app_name)
    {
    }
} // namespace toolbox::workspaces
