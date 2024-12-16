/**
 * @file hotkeys.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 03/12/2024
 */

#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include "hotkeys.hpp"


hotkeys::KeymapEventFilter::KeymapEventFilter(QObject *parent, std::map<std::string, std::function<void()> *> *keymap) : QObject(parent), id_map {}
{
    this->keymap = keymap;
    unsigned int id = 0;
    for (auto &keymap_pair : *keymap) {
        auto keycode = keycode_from_combination(keymap_pair.first);
        auto modifiers = modifiers_from_combination(keymap_pair.first);
        if (RegisterHotKey(nullptr, id, modifiers,keycode)) {
            id_map->insert({id, keymap_pair.second});
            id++;
            qDebug() << std::format("Combination {} registered successfully.", keymap_pair.first);
        } else {
            qDebug() << std::format("Failed to register combination {}.", keymap_pair.first);
        }
    }
}

hotkeys::KeymapEventFilter::~KeymapEventFilter()
{
    // Unregister the hotkey
    UnregisterHotKey(nullptr, 1);
}

// Override native event filter to capture the WM_HOTKEY message
bool hotkeys::KeymapEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY) {
            auto hotkey_id = msg->wParam; // ID of the hotkey
            auto it = std::next(keymap->begin(), hotkey_id);
            qDebug() << std::format("Combination {} pressed.", it->first);
            return true;
        }
    }
    return false;
}

unsigned int hotkeys::KeymapEventFilter::keycode_from_combination(std::string combination)
{
    static const std::map<std::string, int> key_map = {
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
        {"scrlock", VK_SCROLL}
    };

    // TODO error handling
    const std::string key_desc = combination.substr(combination.find("+") + 1);

    return key_map[key_desc];
}

unsigned int hotkeys::KeymapEventFilter::modifiers_from_combination(std::string combination)
{
    unsigned int modifiers = 0;

    if (combination.contains("ctrl")) {
        modifiers |= MOD_CONTROL;
    }

    if (combination.contains("alt")) {
        modifiers |= MOD_ALT;
    }

    if (combination.contains("shift")) {
        modifiers |= MOD_SHIFT;
    }

    if (combination.contains("meta")) {
        modifiers |= MOD_WIN;
    }

    return modifiers;
}

hotkeys::KeymapEventFilter *hotkeys::install_keymap(QApplication *app, std::map<std::string, std::function<void()> *> *keymap)
{
    KeymapEventFilter *event_filter = new KeymapEventFilter(app, keymap);
    app->installNativeEventFilter(event_filter);
    return event_filter;
}

bool hotkeys::remove_keymap(KeymapEventFilter *event_filter)
{
    return false;
}
