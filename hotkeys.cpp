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


HotkeyNativeEventFilter::HotkeyNativeEventFilter(QObject *parent, std::map<Hotkey, std::function<void()>> *keymap) : QObject(parent)
{
    this->keymap = keymap;

    // Register keymap with ids here

    // Register hotkey (ALT + A)
    if (RegisterHotKey(nullptr, 1, MOD_ALT | MOD_NOREPEAT, 0x41)) {  // 0x41 is the virtual key code for 'A'
        qDebug() << "Hotkey 'ALT + A' registered successfully.";
    } else {
        qDebug() << "Failed to register hotkey.";
    }
}

HotkeyNativeEventFilter::~HotkeyNativeEventFilter()
{
    // Unregister the hotkey
    UnregisterHotKey(nullptr, 1);
}

// Override native event filter to capture the WM_HOTKEY message
bool HotkeyNativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY) {
            if (msg->wParam == 1) {  // ID of the hotkey we registered
//                QMessageBox::information(nullptr, "Hotkey Pressed", "ALT + A was pressed!");
                return true;
            }
        }
    }
    return false;
}

void hotkeys_register_event_filter(QApplication *app, std::map<Hotkey, std::function<void()>> *keymap) {
    HotkeyNativeEventFilter *event_filter = new HotkeyNativeEventFilter(app, keymap);
    app->installNativeEventFilter(event_filter);
}