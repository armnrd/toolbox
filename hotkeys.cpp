/**
 * @file hotkeys.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 03/12/2024
 */

#include "hotkeys.hpp"


static std::map<std::string, int> keycode_map = {
        {"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'},
        {"F", 'F'}, {"G", 'G'}, {"H", 'H'}, {"I", 'I'}, {"J", 'J'},
        {"K", 'K'}, {"L", 'L'}, {"M", 'M'}, {"N", 'N'}, {"O", 'O'},
        {"P", 'P'}, {"Q", 'Q'}, {"R", 'R'}, {"S", 'S'}, {"T", 'T'},
        {"U", 'U'}, {"V", 'V'}, {"W", 'W'}, {"X", 'X'}, {"Y", 'Y'},
        {"Z", 'Z'},
        {"0", '0'}, {"1", '1'}, {"2", '2'}, {"3", '3'}, {"4", '4'},
        {"5", '5'}, {"6", '6'}, {"7", '7'}, {"8", '8'}, {"9", '9'},
        {"return", VK_RETURN}, {"ctrl", VK_CONTROL}, {"alt", VK_MENU},
        {"shift", VK_SHIFT}, {"tab", VK_TAB}, {"space", VK_SPACE},
        {"esc", VK_ESCAPE}, {"up", VK_UP}, {"down", VK_DOWN},
        {"left", VK_LEFT}, {"right", VK_RIGHT}, {"bksp", VK_BACK},
        {"del", VK_DELETE}, {"ins", VK_INSERT}, {"home", VK_HOME},
        {"end", VK_END}, {"pgup", VK_PRIOR}, {"pgdown", VK_NEXT},
        {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4},
        {"F5", VK_F5}, {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8},
        {"F9", VK_F9}, {"F10", VK_F10}, {"F11", VK_F11}, {"F12", VK_F12},
        {"capslock", VK_CAPITAL}, {"numlock", VK_NUMLOCK},
        {"scrlock", VK_SCROLL}, {"plus", VK_OEM_PLUS},
        {"comma", VK_OEM_COMMA}, {"minus", VK_OEM_MINUS},
        {"period", VK_OEM_PERIOD}
};

static std::map<std::string, int> modifier_map = {
        {"shift", MOD_SHIFT}, {"ctrl", MOD_CONTROL},
        {"meta", MOD_WIN}, {"alt", MOD_ALT}
};

// Map from function names to functions
static std::map<std::string, std::function<void()>> function_map = {
        {"window_management::action_minimize", window_management::action_minimize},
        {"window_management::action_toggle_maximize", window_management::action_toggle_maximize},
        {"window_management::action_close", window_management::action_close},
        {"window_management::action_resize", window_management::action_resize},
        {"window_management::position_centre", window_management::position_centre},
        {"window_management::position_lower_screen", window_management::position_lower_screen},
        {"window_management::position_upper_screen", window_management::position_upper_screen},
        {"window_management::position_right_middle", window_management::position_right_middle},
        {"window_management::position_top_right", window_management::position_top_right},
        {"window_management::position_top_middle", window_management::position_top_middle},
        {"window_management::position_top_left", window_management::position_top_left},
        {"window_management::position_left_middle", window_management::position_left_middle},
        {"window_management::position_bottom_left", window_management::position_bottom_left},
        {"window_management::position_bottom_middle", window_management::position_bottom_middle},
        {"window_management::position_bottom_right", window_management::position_bottom_right},
        {"window_management::move_fast_left", window_management::move_fast_left},
        {"window_management::move_slow_left", window_management::move_slow_left},
        {"window_management::move_fast_right", window_management::move_fast_right},
        {"window_management::move_slow_right", window_management::move_slow_right},
        {"window_management::move_fast_up", window_management::move_fast_up},
        {"window_management::move_slow_up", window_management::move_slow_up},
        {"window_management::move_fast_down", window_management::move_fast_down},
        {"window_management::move_slow_down", window_management::move_slow_down}
};

hotkeys::KeyMapEventFilter::KeyMapEventFilter(KeyMap *key_map)
{
    this->key_map = key_map;
    this->id_map = new IDMap();
    unsigned int id = 0;
    for (auto &pair : *key_map) {
        auto key_code = keycode_from_description(pair.first);
        auto modifiers = modifiers_from_description(pair.first);
        if (RegisterHotKey(nullptr, id, modifiers, key_code)) {
            id_map->insert({id, pair.second});
            id++;
            qDebug() << std::format("Combination {} registered successfully: {:#X} {:#X}", pair.first, key_code, modifiers);
        } else {
            qDebug() << std::format("Failed to register combination {}: {:#X} {:#X}", pair.first, key_code, modifiers);
        }
    }
}

hotkeys::KeyMapEventFilter::~KeyMapEventFilter()
{
    // TODO placeholder
}

// Override native event filter to capture the WM_HOTKEY message
bool hotkeys::KeyMapEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY) {
            auto hotkey_id = msg->wParam; // ID of the hotkey
            (*id_map)[hotkey_id]();
            return true;
        }
    }
    return false;
}

unsigned int hotkeys::KeyMapEventFilter::keycode_from_description(std::string description)
{
    // TODO error handling
    std::string key_desc = description.substr(description.rfind("+") + 1);

    return keycode_map[key_desc];
}

unsigned int hotkeys::KeyMapEventFilter::modifiers_from_description(std::string description)
{
    unsigned int modifiers = 0;

    // TODO error handling
    for (auto &pair : modifier_map) {
        if (description.contains(pair.first)) {
            modifiers |= pair.second;
        }
    }

    return modifiers;
}

/**
 * @brief Installs a KeyMap.
 *
 * @param key_map - KeyMap to register with the event filter
 * @param app - reference to main application; the KeyMap will be installed on this
 * @note placeholder
 */
void hotkeys::Factory::set_keymap(hotkeys::KeyMap *key_map, QApplication *app)
{
    // If a KeyMapEventFilter was already installed on app, remove it
    if (key_map_event_filter) {
        app->removeNativeEventFilter(key_map_event_filter);
    }

    KeyMapEventFilter *event_filter = new KeyMapEventFilter(key_map);
    app->installNativeEventFilter(event_filter);
}

hotkeys::KeyMap *hotkeys::keymap_from_config(config::Config *config)
{
    auto key_map = new hotkeys::KeyMap();

    // Use iterators to iterate over Window management key combinations
    for (auto it = (*config)["hotkeys"]["window"].begin(); it != (*config)["hotkeys"]["window"].end(); ++it) {
        for (auto jt = it.value().begin(); jt != it.value().end(); ++jt) {
            auto function_name = std::format("window_management::{}_{}", it.key(), jt.key());
            // TODO error handling
            key_map->insert({jt.value(), function_map[function_name]});
        }
    }

    return key_map;
}

