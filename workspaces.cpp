#include "workspaces.hpp"
#include <QLabel>
#include <windows.h>
#include <tlhelp32.h>

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
    wstring toggle_window;
};

static map<string, map<string, App> > spaces = {};

namespace YAML
{
    template<>
    struct convert<App>
    {
        static Node encode(const App &app)
        {
            // TODO: handle null values
            Node node;
            node["location"] = string(app.location.begin(), app.location.end());
            node["executable"] = string(app.executable.begin(), app.executable.end());
            node["args"] = string(app.args.begin(), app.args.end());
            node["position"] = string(app.position.begin(), app.position.end());
            node["toggle window"] = string(app.toggle_window.begin(), app.toggle_window.end());
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
            auto toggle_window_str = node["toggle window"].as<string>();

            app.location = wstring(location_str.begin(), location_str.end());
            app.executable = wstring(executable_str.begin(), executable_str.end());
            app.args = wstring(args_str.begin(), args_str.end());
            app.position = wstring(position_str.begin(), position_str.end());
            app.toggle_window = wstring(toggle_window_str.begin(), toggle_window_str.end());

            qDebug() << format(
                L"Decoded App: location: '{}', executable: '{}', args: '{}', position: '{}', toggle window: '{}'",
                app.location, app.executable, app.args, app.position, app.toggle_window);

            return true;
        }
    };
} // namespace YAML

static std::list<string> get_running_processes()
{
    std::logic_error("Not implemented");
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

static DWORD launch_process(const wstring &location, const wstring &executable, const wstring &args)
{
    qDebug() << format(L"Launching process: {}", executable);
    auto path = location + L"\\" + executable;
    auto command_line = path + L" " + args;
    STARTUPINFOW si = {sizeof(STARTUPINFOW)};
    PROCESS_INFORMATION pi;
    if (!CreateProcessW(path.data(), command_line.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, location.data(),
                        &si, &pi))
    {
        auto message = std::system_category().message(GetLastError());
        qDebug() << format(L"Failed to launch process: {}; error: {} (GetLastError: {})",
                           executable, wstring(message.begin(), message.end()) , GetLastError());
        // TODO: handle failure
        return 0;
    }
    qDebug() << format(L"Process launched: {}; pid: {}", executable, pi.dwProcessId);

    auto pid = pi.dwProcessId;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return pid;
}

static HWND hwnd_from_pid(DWORD pid)
{
    HWND hwnd = nullptr;
    auto packvar = std::pair(&hwnd, pid);
    EnumWindows(
        [](HWND win_hwnd, LPARAM lp) -> BOOL
        {
            auto pack = *(reinterpret_cast<std::pair<HWND *, DWORD> *>(lp));
            DWORD wndPid;
            GetWindowThreadProcessId(win_hwnd, &wndPid);
            if (wndPid == pack.second && GetWindow(win_hwnd, GW_OWNER) == nullptr && IsWindowVisible(win_hwnd))
            {
                pack.first = &win_hwnd;
                return FALSE;
            }
            return TRUE;
        },
        LPARAM(&packvar)
    );
    qDebug() << format(L"Main window HWND: {}", (uintptr_t) hwnd);

    return hwnd;
}

namespace toolbox::workspaces
{
    void launch_terminal()
    {
        auto pid = launch_process(L"C:\\Users\\biswas02\\AppData\\Local\\Microsoft\\WindowsApps",
        L"wt.exe", L"");

        Sleep(5000);

        auto hwnd = hwnd_from_pid(pid);

        qDebug() << format(L"Terminal window HWND: {} | IsWindow: {} | IsVisible: {}",
                               (uintptr_t)hwnd,
                               hwnd ? IsWindow(hwnd) : 0,
                               hwnd ? IsWindowVisible(hwnd) : 0);
        // position: bottom-centre
        // toggle window: ctrl+meta+comma
      }

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
        // TODO save app lists to config on destruction
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
        std::logic_error("Not implemented");
    }

    void Workspaces::remove_app_from_base_list(const string &app_name)
    {
        std::logic_error("Not implemented");
    }

    void Workspaces::launch_profile_apps(const string &profile_name)
    {
        std::logic_error("Not implemented");
    }

    std::list<string> *Workspaces::get_profile_app_list(const string &profile_name)
    {
        std::logic_error("Not implemented");
        return nullptr;
    }

    void Workspaces::add_app_to_profile_list(const string &profile_name, const string &app_name)
    {
        std::logic_error("Not implemented");
    }

    void Workspaces::remove_app_from_profile_list(const string &profile_name, const string &app_name)
    {
        std::logic_error("Not implemented");
    }
} // namespace toolbox::workspaces
