/**
 * @file hotkeys.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 03/12/2024
 */

#ifndef TOOLBOX_HOTKEYS_HPP
#define TOOLBOX_HOTKEYS_HPP

struct Hotkey {
    int id;                 // Unique identifier for the hotkey
    int key;                // Virtual key code (e.g., 0x41 for 'A')
    unsigned int modifiers; // Modifier keys (e.g., MOD_ALT | MOD_CONTROL)
};

class HotkeyNativeEventFilter : public QObject, public QAbstractNativeEventFilter {
public:
    HotkeyNativeEventFilter(QObject* parent, std::map<Hotkey, std::function<void()>> *keymap);

    ~HotkeyNativeEventFilter();

    // Override native event filter to capture the WM_HOTKEY message
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    std::map<Hotkey, std::function<void()>> *keymap;
};

void hotkeys_register_event_filter(QApplication *app, std::map<Hotkey, std::function<void()>> *keymap);

#endif //TOOLBOX_HOTKEYS_HPP
