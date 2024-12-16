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

unsigned int hotkeys::KeymapEventFilter::keycode_from_combination(std::string)
{
    return 0;
}

unsigned int hotkeys::KeymapEventFilter::modifiers_from_combination(std::string)
{
    return 0;
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
