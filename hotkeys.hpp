/**
 * @file hotkeys.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 03/12/2024
 */

#ifndef TOOLBOX_HOTKEYS_HPP
#define TOOLBOX_HOTKEYS_HPP

namespace hotkeys
{
    class KeymapEventFilter : public QObject, public QAbstractNativeEventFilter {
    public:
        KeymapEventFilter(QObject* parent, std::map<std::string, std::function<void()> *> *keymap);

        ~KeymapEventFilter();

        // Override native event filter to capture the WM_HOTKEY message
        bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

    private:
        // (key combination, (function, hotkey id)
        std::map<std::string, std::function<void()> *> *keymap;
        std::map<unsigned int, std::function<void()> *> *id_map;
        unsigned int keycode_from_combination(std::string);
        unsigned int modifiers_from_combination(std::string);
    };
    
    /**
     * @brief Registers a keymap.
     *
     * @param app - reference to main application
     * @param keymap - keymap to register with the event filter
     * @return the NativeEventFilter object generated from the keymap; for use later if the keymap needs changing
     * @note placeholder
     */
    KeymapEventFilter *install_keymap(QApplication *app, std::map<std::string, std::function<void()> *> *keymap);

    bool remove_keymap(KeymapEventFilter *event_filter);

}

#endif //TOOLBOX_HOTKEYS_HPP
