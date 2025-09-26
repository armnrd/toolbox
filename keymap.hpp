#ifndef TOOLBOX_KEYMAP_HPP
#define TOOLBOX_KEYMAP_HPP

#include <QAbstractNativeEventFilter>
#include <QApplication>
#include <functional>
#include <map>
#include <string>
#include "config.hpp"

// (keybind description, action)
typedef std::map<std::string, std::function<void()>> Keymap;
// (keybind id, (keybind description, action))
typedef std::map<unsigned int, std::pair<std::string, std::function<void()>>> IDMap;

class KeymapEventFilter : public QObject, public QAbstractNativeEventFilter
{
public:
    KeymapEventFilter() = default;

    ~KeymapEventFilter() override;

    // Override native event filter to capture the WM_HOTKEY message
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

    IDMap *_id_map = new IDMap();
    int _next_id = 0;
};

namespace toolbox::keymap
{
    class KeymapMgr
    {
    public:
        KeymapMgr(QApplication *app);
        void add_keybind(std::string description, std::function<void()> action);
        void add_keybinds(Keymap *keymap);
        void add_keybinds_from_config(config::Config *config);
        void remove_keybind(std::string description);
        void remove_all_keybinds();

    private:
        QApplication *app = nullptr;
        KeymapEventFilter *keymap_event_filter = nullptr;
    };
}

#endif // TOOLBOX_KEYMAP_HPP
