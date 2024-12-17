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

hotkeys::KeyMapEventFilter::KeyMapEventFilter(QObject *parent, KeyMap *key_map) : QObject(parent)
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
            qDebug() << std::format("Combination {} registered successfully.", pair.first);
        } else {
            qDebug() << std::format("Failed to register combination {}.", pair.first);
        }
    }
}

hotkeys::KeyMapEventFilter::~KeyMapEventFilter()
{
    // Unregister the hotkey
    UnregisterHotKey(nullptr, 1);
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
 * @param app - reference to main application
 * @param key_map - KeyMap to register with the event filter
 * @return the NativeEventFilter object generated from the key_map; for use later if the key_map needs changing
 * @note placeholder
 */
hotkeys::KeyMapEventFilter *hotkeys::install_keymap(QApplication *app, KeyMap *key_map)
{
    KeyMapEventFilter *event_filter = new KeyMapEventFilter(app, key_map);
    app->installNativeEventFilter(event_filter);
    return event_filter;
}

void hotkeys::remove_keymap(QApplication *app, KeyMapEventFilter *event_filter)
{
    app->removeEventFilter(event_filter);
}
