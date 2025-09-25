#ifndef TOOLBOX_HOTKEYS_HPP
#define TOOLBOX_HOTKEYS_HPP

#include <QAbstractNativeEventFilter>
#include <QApplication>
#include <functional>
#include <map>
#include <string>
#include "config.hpp"

// (hotkey description, action)
typedef std::map<std::string, std::function<void()>> KeyMap;
// (hotkey id, (hotkey description, action))
typedef std::map<unsigned int, std::pair<std::string, std::function<void()>>> IDMap;

class KeyMapEventFilter : public QObject, public QAbstractNativeEventFilter
{
public:
    KeyMapEventFilter() = default;

    ~KeyMapEventFilter() override;

    void register_keymap(KeyMap *key_map);

    void register_hotkey(std::string description, std::function<void()> action);

    void unregister_hotkey(std::string description);

    // Override native event filter to capture the WM_HOTKEY message
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    // (hotkey id, action)
    IDMap *id_map = new IDMap();
    int next_id = 0;
};

namespace toolbox::hotkeys
{
    class Hotkeys
    {
    public:
        Hotkeys(QApplication *app);
        void add_hotkeys_from_config(config::Config *config);
        void add_hotkey(std::string description, std::function<void()> action);
        void remove_hotkey(std::string description);
        void reset_hotkeys();

    private:
        QApplication *app = nullptr;
        KeyMapEventFilter *keymap_event_filter = nullptr;
    };
}

#endif // TOOLBOX_HOTKEYS_HPP
