#ifndef TOOLBOX_HOTKEYS_HPP
#define TOOLBOX_HOTKEYS_HPP

#include <QAbstractNativeEventFilter>
#include <QApplication>
#include <functional>
#include <map>
#include <string>
#include "config.hpp"

namespace toolbox::hotkeys
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
    };

    class Hotkeys
    {
    public:
        Hotkeys(QApplication *app, config::Config *config);
        void register_hotkeys();
        void unregister_hotkeys();
        // TODO: individual add and remove functions

    private:
        QApplication *app = nullptr;
        KeyMap *key_map = nullptr;
        KeyMapEventFilter *key_map_event_filter = nullptr;
    };
} // namespace toolbox::hotkeys

#endif // TOOLBOX_HOTKEYS_HPP
