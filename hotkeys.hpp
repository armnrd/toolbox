/**
 * @file hotkeys.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 03/12/2024
 */

#ifndef TOOLBOX_HOTKEYS_HPP
#define TOOLBOX_HOTKEYS_HPP

#include "config.hpp"
#include "window_management.hpp"
#include <map>
#include <string>
#include <functional>
#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <windows.h>

namespace hotkeys
{
    typedef std::map<std::string, std::function<void()>> KeyMap;
    typedef std::map<unsigned int, std::function<void()>> IDMap;

    class KeyMapEventFilter : public QObject, public QAbstractNativeEventFilter
    {
    public:
        KeyMapEventFilter(std::map<std::string, std::function<void()>> *key_map);

        ~KeyMapEventFilter();

        // Override native event filter to capture the WM_HOTKEY message
        bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

    private:
        // (key combination, (function, hotkey id)
        KeyMap *key_map;
        IDMap *id_map;
        unsigned int keycode_from_description(std::string);
        unsigned int modifiers_from_description(std::string);
    };

    class Factory
    {
    public:
        void set_keymap(KeyMap *key_map, QApplication *app);

    private:
        KeyMap *key_map = nullptr;
        KeyMapEventFilter *key_map_event_filter = nullptr;
    };

    KeyMap *keymap_from_config(config::Config *config);
}

#endif //TOOLBOX_HOTKEYS_HPP
