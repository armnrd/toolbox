#include "keymap.hpp"
#include <windows.h>
#include "window_management.hpp"

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
    {"window_management::action_minimize", toolbox::window_management::action_minimize},
    {"window_management::action_toggle_maximize", toolbox::window_management::action_toggle_maximize},
    {"window_management::action_close", toolbox::window_management::action_close},
    {"window_management::action_resize", toolbox::window_management::action_resize},
    {"window_management::position_centre", toolbox::window_management::position_centre},
    {"window_management::position_lower_screen", toolbox::window_management::position_lower_screen},
    {"window_management::position_upper_screen", toolbox::window_management::position_upper_screen},
    {"window_management::position_right_middle", toolbox::window_management::position_right_middle},
    {"window_management::position_top_right", toolbox::window_management::position_top_right},
    {"window_management::position_top_middle", toolbox::window_management::position_top_middle},
    {"window_management::position_top_left", toolbox::window_management::position_top_left},
    {"window_management::position_left_middle", toolbox::window_management::position_left_middle},
    {"window_management::position_bottom_left", toolbox::window_management::position_bottom_left},
    {"window_management::position_bottom_middle", toolbox::window_management::position_bottom_middle},
    {"window_management::position_bottom_right", toolbox::window_management::position_bottom_right},
    {"window_management::move_fast_left", toolbox::window_management::move_fast_left},
    {"window_management::move_slow_left", toolbox::window_management::move_slow_left},
    {"window_management::move_fast_right", toolbox::window_management::move_fast_right},
    {"window_management::move_slow_right", toolbox::window_management::move_slow_right},
    {"window_management::move_fast_up", toolbox::window_management::move_fast_up},
    {"window_management::move_slow_up", toolbox::window_management::move_slow_up},
    {"window_management::move_fast_down", toolbox::window_management::move_fast_down},
    {"window_management::move_slow_down", toolbox::window_management::move_slow_down}
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

    // Use iterators to iterate over Window management keybinds
    for (auto it = (*config)["keymap"]["window"].begin(); it != (*config)["keymap"]["window"].end(); ++it)
    {
        for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
        {
            auto function_name = std::format("window_management::{}_{}", it->first.as<std::string>(),
                                             jt->first.as<std::string>());
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
        MSG *msg = static_cast<MSG *>(message);
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
        keymap_event_filter = new KeymapEventFilter();
        app->installNativeEventFilter(keymap_event_filter);
    }

    void KeymapMgr::add_keybind(std::string description, std::function<void()> action)
    {
        auto keycode = keycode_from_description(description);
        auto modifiers = modifiers_from_description(description);
        if (RegisterHotKey(nullptr, keymap_event_filter->_next_id, modifiers, keycode))
        {
            keymap_event_filter->_id_map->insert({keymap_event_filter->_next_id, {description, action}});
            keymap_event_filter->_next_id++;
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
        // Iterate over pairs in keymap_event_filter._id_map and remove the one that matches description
        for (auto it = keymap_event_filter->_id_map->begin(); it != keymap_event_filter->_id_map->end(); ++it)
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
                keymap_event_filter->_id_map->erase(it);
                return;
            }
        }
    }

    void KeymapMgr::remove_all_keybinds()
    {
        // Iterate over keymap_event_iflter._id_map and unregister each keybind
        for (auto it = keymap_event_filter->_id_map->begin(); it != keymap_event_filter->_id_map->end(); ++it)
        {
            if (UnregisterHotKey(nullptr, it->first))
            {
                qDebug() << std::format("Keybind {} unregistered successfully.", it->second.first);
            } else
            {
                qDebug() << std::format("Failed to unregister keybind {}.", it->second.first);
            }
        }
        // Clear keymap_event_filter._id_map
        keymap_event_filter->_id_map->clear();
        keymap_event_filter->_next_id = 0;
    }
}
