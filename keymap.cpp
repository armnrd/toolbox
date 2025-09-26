#include "keymap.hpp"
#include <windows.h>
#include "windowmgr.hpp"

static std::map<std::string, int> keycode_map = {
    {"A", 'A'},
    {"B", 'B'},
    {"C", 'C'},
    {"D", 'D'},
    {"E", 'E'},
    {"F", 'F'},
    {"G", 'G'},
    {"H", 'H'},
    {"I", 'I'},
    {"J", 'J'},
    {"K", 'K'},
    {"L", 'L'},
    {"M", 'M'},
    {"N", 'N'},
    {"O", 'O'},
    {"P", 'P'},
    {"Q", 'Q'},
    {"R", 'R'},
    {"S", 'S'},
    {"T", 'T'},
    {"U", 'U'},
    {"V", 'V'},
    {"W", 'W'},
    {"X", 'X'},
    {"Y", 'Y'},
    {"Z", 'Z'},
    {"0", '0'},
    {"1", '1'},
    {"2", '2'},
    {"3", '3'},
    {"4", '4'},
    {"5", '5'},
    {"6", '6'},
    {"7", '7'},
    {"8", '8'},
    {"9", '9'},
    {"return", VK_RETURN},
    {"ctrl", VK_CONTROL},
    {"alt", VK_MENU},
    {"shift", VK_SHIFT},
    {"tab", VK_TAB},
    {"space", VK_SPACE},
    {"esc", VK_ESCAPE},
    {"up", VK_UP},
    {"down", VK_DOWN},
    {"left", VK_LEFT},
    {"right", VK_RIGHT},
    {"bksp", VK_BACK},
    {"del", VK_DELETE},
    {"ins", VK_INSERT},
    {"home", VK_HOME},
    {"end", VK_END},
    {"pgup", VK_PRIOR},
    {"pgdown", VK_NEXT},
    {"F1", VK_F1},
    {"F2", VK_F2},
    {"F3", VK_F3},
    {"F4", VK_F4},
    {"F5", VK_F5},
    {"F6", VK_F6},
    {"F7", VK_F7},
    {"F8", VK_F8},
    {"F9", VK_F9},
    {"F10", VK_F10},
    {"F11", VK_F11},
    {"F12", VK_F12},
    {"capslock", VK_CAPITAL},
    {"numlock", VK_NUMLOCK},
    {"scrlock", VK_SCROLL},
    {"plus", VK_OEM_PLUS},
    {"comma", VK_OEM_COMMA},
    {"minus", VK_OEM_MINUS},
    {"period", VK_OEM_PERIOD}
};

static std::map<std::string, int> modifier_map = {
    {"shift", MOD_SHIFT}, {"ctrl", MOD_CONTROL}, {"meta", MOD_WIN}, {"alt", MOD_ALT}
};

// Map from function names to functions
static std::map<std::string, std::function<void()> > function_map = {
    {"action_minimize", toolbox::windowmgr::action_foreground_minimize},
    {"action_toggle_maximize", toolbox::windowmgr::action_foreground_toggle_maximize},
    {"action_close", toolbox::windowmgr::action_foreground_close},
    {"action_resize", toolbox::windowmgr::action_foreground_resize},
    {"position_centre", toolbox::windowmgr::position_foreground_centre},
    {"position_lower_screen", toolbox::windowmgr::position_foreground_lower_screen},
    {"position_upper_screen", toolbox::windowmgr::position_foreground_upper_screen},
    {"position_right_middle", toolbox::windowmgr::position_foreground_right_middle},
    {"position_top_right", toolbox::windowmgr::position_foreground_top_right},
    {"position_top_middle", toolbox::windowmgr::position_foreground_top_middle},
    {"position_top_left", toolbox::windowmgr::position_foreground_top_left},
    {"position_left_middle", toolbox::windowmgr::position_foreground_left_middle},
    {"position_bottom_left", toolbox::windowmgr::position_foreground_bottom_left},
    {"position_bottom_middle", toolbox::windowmgr::position_foreground_bottom_middle},
    {"position_bottom_right", toolbox::windowmgr::position_foreground_bottom_right},
    {"move_fast_left", toolbox::windowmgr::move_foreground_fast_left},
    {"move_slow_left", toolbox::windowmgr::move_foreground_slow_left},
    {"move_fast_right", toolbox::windowmgr::move_foreground_fast_right},
    {"move_slow_right", toolbox::windowmgr::move_foreground_slow_right},
    {"move_fast_up", toolbox::windowmgr::move_foreground_fast_up},
    {"move_slow_up", toolbox::windowmgr::move_foreground_slow_up},
    {"move_fast_down", toolbox::windowmgr::move_foreground_fast_down},
    {"move_slow_down", toolbox::windowmgr::move_foreground_slow_down}
};

static unsigned int keycode_from_description(std::string description)
{
    // TODO error handling
    std::string key_desc = description.substr(description.rfind("+") + 1);

    return keycode_map[key_desc];
}

static unsigned int modifiers_from_description(std::string description)
{
    unsigned int modifiers = 0;

    // TODO error handling
    for (auto &pair: modifier_map)
    {
        if (description.contains(pair.first))
        {
            modifiers |= pair.second;
        }
    }

    return modifiers;
}

static Keymap *keymap_from_config(toolbox::config::Config *config)
{
    auto keymap = new Keymap();

    // Use iterators to iterate over window manager keybinds
    for (auto it = (*config)["keymap"]["windowmgr"].begin(); it != (*config)["keymap"]["windowmgr"].end(); ++it)
    {
        for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
        {
            auto function_name = std::format("{}_{}", it->first.as<std::string>(), jt->first.as<std::string>());
            // TODO error handling
            keymap->insert({jt->second.as<std::string>(), function_map[function_name]});
        }
    }

    return keymap;
}

KeymapEventFilter::~KeymapEventFilter()
{
    delete _id_map;
}

// Override native event filter to capture the WM_HOTKEY message
bool KeymapEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        auto *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY)
        {
            auto keybind_id = msg->wParam; // ID of the keybind
            if (keybind_id < _id_map->size())
            {
                (*_id_map)[keybind_id].second();
                return true;
            }
        }
    }
    return false;
}

namespace toolbox::keymap
{
    /**
     * @brief  Sets up keymap.
     *
     * @param config - reference to configuration; key map will be read from this
     * @param app - reference to main application; key map will be installed on this
     * @note placeholder
     */
    KeymapMgr::KeymapMgr(QApplication *app)
    {
        kmef = new KeymapEventFilter();
        app->installNativeEventFilter(kmef);
    }

    void KeymapMgr::add_keybind(std::string description, std::function<void()> action)
    {
        auto keycode = keycode_from_description(description);
        auto modifiers = modifiers_from_description(description);
        if (RegisterHotKey(nullptr, kmef->_next_id, modifiers, keycode))
        {
            kmef->_id_map->insert({kmef->_next_id, {description, action}});
            kmef->_next_id++;
            qDebug() << std::format("Keybind {} registered successfully: {:#X} {:#X}", description, keycode,
                                    modifiers);
        } else
        {
            qDebug() << std::format("Failed to register keybind {}: {:#X} {:#X}", description, keycode,
                                    modifiers);
        }
    }

    void KeymapMgr::add_keybinds_from_config(config::Config *config)
    {
        auto keymap = keymap_from_config(config);
        for (auto &pair: *keymap)
        {
            add_keybind(pair.first, pair.second);
        }
    }

    void KeymapMgr::remove_keybind(std::string description)
    {
        // Iterate over pairs in kmef._id_map and remove the one that matches description
        for (auto it = kmef->_id_map->begin(); it != kmef->_id_map->end(); ++it)
        {
            if (it->second.first == description)
            {
                if (UnregisterHotKey(nullptr, it->first))
                {
                    qDebug() << std::format("Keybind {} unregistered successfully.", description);
                } else
                {
                    qDebug() << std::format("Failed to unregister keybind {}.", description);
                }
                kmef->_id_map->erase(it);
                return;
            }
        }
    }

    void KeymapMgr::remove_all_keybinds()
    {
        // Iterate over keymap_event_iflter._id_map and unregister each keybind
        for (auto it = kmef->_id_map->begin(); it != kmef->_id_map->end(); ++it)
        {
            if (UnregisterHotKey(nullptr, it->first))
            {
                qDebug() << std::format("Keybind {} unregistered successfully.", it->second.first);
            } else
            {
                qDebug() << std::format("Failed to unregister keybind {}.", it->second.first);
            }
        }
        // Clear kmef._id_map
        kmef->_id_map->clear();
        kmef->_next_id = 0;
    }
}
