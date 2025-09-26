#include "workspaces.hpp"
#include "windowmgr.hpp"
#include "keymap.hpp"
#include "tray.hpp"
#include <psapi.h>
#include <QLabel>
#include <thread>
#include <windows.h>
#include <tlhelp32.h>

using std::format;
using std::map;
using std::string;
using std::wstring;

static std::map<std::string, std::function<void(HWND)> > windowmgr_function_map = {
    {"centre", toolbox::windowmgr::centre},
    {"lower_screen", toolbox::windowmgr::position_lower_screen},
    {"upper_screen", toolbox::windowmgr::position_upper_screen},
    {"right_middle", toolbox::windowmgr::position_right_middle},
    {"top_right", toolbox::windowmgr::position_top_right},
    {"top_middle", toolbox::windowmgr::position_top_middle},
    {"top_left", toolbox::windowmgr::position_top_left},
    {"left_middle", toolbox::windowmgr::position_left_middle},
    {"bottom_left", toolbox::windowmgr::position_bottom_left},
    {"bottom_middle", toolbox::windowmgr::position_bottom_middle},
    {"bottom_right", toolbox::windowmgr::position_bottom_right},
};

static std::list<string> get_running_processes()
{
    std::logic_error("Not implemented");
}

static bool is_process_running(const wstring &exe_name)
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
        if (exe_name == pe32.szExeFile)
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
                           executable, wstring(message.begin(), message.end()), GetLastError());
        // TODO: handle failure
        return 0;
    }
    qDebug() << format(L"Process launched: {}; pid: {}", executable, pi.dwProcessId);

    auto pid = pi.dwProcessId;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return pid;
}

static wstring exe_path_from_pid(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess)
    {
        wchar_t exePath[MAX_PATH] = {};
        if (GetModuleFileNameExW(hProcess, nullptr, exePath, MAX_PATH))
        {
            CloseHandle(hProcess);
            return exePath;
        }
        CloseHandle(hProcess);
    }
    return {};
}

static HWND hwnd_from_exe_name(const wstring &exe_name)
{
    HWND hwnd = nullptr;
    // (address of variable hwnd, executable name)
    auto packvar = std::make_pair(&hwnd, exe_name);
    EnumWindows(
        [](HWND win_hwnd, LPARAM packvar_ptr) -> BOOL
        {
            auto pack = *(reinterpret_cast<std::pair<HWND *, const wstring> *>(packvar_ptr));
            DWORD win_pid;

            // Retrieve the name of the executable that created win_hwnd
            GetWindowThreadProcessId(win_hwnd, &win_pid);
            auto win_exe_path = exe_path_from_pid(win_pid);

            if (pack.second == win_exe_path.substr(win_exe_path.find_last_of(L"\\") + 1))
            {
                *(pack.first) = win_hwnd;
                return FALSE; // Stop enumeration
            }

            return TRUE;
        },
        LPARAM(&packvar)
    );
    qDebug() << format(L"HWND of window matching {}: {}", exe_name, (uintptr_t) hwnd);

    return hwnd;
}

void app_post_launch(HWND hwnd, const AppConfig &ac, toolbox::keymap::KeymapMgr *kmmgr)
{
    qDebug() << "In post launch.";

    if (ac.position != "null")
    {
        // TODO error handling
        windowmgr_function_map[ac.position](hwnd);
        qDebug() << format("Positioning done: {}", ac.position);
    }

    if (ac.toggle_window != "null")
    {
        toolbox::tray::AppTrayIcon *tray_icon = nullptr;
        QMetaObject::invokeMethod(QApplication::instance(), [&]() {
            tray_icon = toolbox::tray::create_app_tray_icon(hwnd);
            qDebug() << format(L"Created tray icon for {}.", ac.window_identifier);
        }, Qt::BlockingQueuedConnection);

        kmmgr->add_keybind(ac.toggle_window,
                           [ac, tray_icon]()
                           {
                               qDebug() << format(L"Keybinding triggered for {}.", ac.window_identifier);
                               tray_icon->toggle_window();
                           }
        );
    }
}

// Waits for window matching exe_name and calls task(window HWND, ... args)
static void winwait_then_post_launch(const AppConfig &ac, toolbox::keymap::KeymapMgr *kmmgr)
{
    const int MAX_ATTEMPTS = 20;
    std::thread([ac, kmmgr]()
    {
        HWND hwnd = nullptr;
        int i = 0;
        while (!hwnd && i < MAX_ATTEMPTS)
        {
            Sleep(400);
            hwnd = hwnd_from_exe_name(ac.window_identifier);
            ++i;
        }
        app_post_launch(hwnd, ac, kmmgr);
    }).detach();
}

namespace toolbox::workspaces
{
    void WorkspaceMgr::launch_terminal()
    {
        AppConfig ac;
        for (auto pair: workspaces["base"])
        {
            if (pair.second.window_identifier == L"WindowsTerminal.exe")
            {
                ac = pair.second;
            }
        }

        launch_app(ac);
    }

    WorkspaceMgr::WorkspaceMgr(config::Config *config, keymap::KeymapMgr *kmmgr)
    {
        workspaces = {};
        this->kmmgr = kmmgr;

        auto base_workspace = map<string, AppConfig>{};
        for (auto it = (*config)["workspaces"]["base"].begin(); it != (*config)["workspaces"]["base"].end(); ++it)
        {
            auto base_app = it->second.as<AppConfig>();
            base_workspace[it->first.as<string>()] = base_app;
        }
        workspaces["base"] = base_workspace;
    }

    WorkspaceMgr::~WorkspaceMgr()
    {
        // TODO save app lists to config on destruction
    }

    void WorkspaceMgr::launch_base_apps()
    {
        qDebug() << "Launching base apps:";
        for (auto pair: workspaces["base"])
        {
            launch_app(pair.second);
        }
    }

    std::list<string> *WorkspaceMgr::get_base_app_list()
    {
        return nullptr;
    }

    void WorkspaceMgr::add_app_to_base_list(const string &app_name)
    {
        std::logic_error("Not implemented");
    }

    void WorkspaceMgr::remove_app_from_base_list(const string &app_name)
    {
        std::logic_error("Not implemented");
    }

    void WorkspaceMgr::launch_profile_apps(const string &profile_name)
    {
        std::logic_error("Not implemented");
    }

    std::list<string> *WorkspaceMgr::get_profile_app_list(const string &profile_name)
    {
        std::logic_error("Not implemented");
        return nullptr;
    }

    void WorkspaceMgr::add_app_to_profile_list(const string &profile_name, const string &app_name)
    {
        std::logic_error("Not implemented");
    }

    void WorkspaceMgr::remove_app_from_profile_list(const string &profile_name, const string &app_name)
    {
        std::logic_error("Not implemented");
    }

    void WorkspaceMgr::launch_app(const AppConfig &ac)
    {
        if (is_process_running(ac.executable))
        {
            qDebug() << format(L"{} already running.", ac.executable);
        } else
        {
            launch_process(ac.location, ac.executable, ac.args);
            qDebug() << format(L"{} launched.", ac.executable);

            if (ac.position == "null" && ac.toggle_window == "null")
            {
                return; // Nothing else to do, return
            }

            winwait_then_post_launch(ac, kmmgr);
        }
    }
} // namespace toolbox::workspaces
