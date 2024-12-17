#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <QPushButton>
#include <windows.h>
#include <QMessageBox>
#include <iostream>
#include "config.hpp"
#include "hotkeys.hpp"
#include "window_management.hpp"
#include "tray_icon.hpp"

void ctrl_meta_alt_O()
{
    qDebug() << "ctrl+meta+alt+O pressed.";
}

int main(int argc, char *argv[])
{
    try {
        QApplication app(argc, argv);

        auto cfg = config::load_config("config.yml");
        auto key_map = config::keymap_from_config(cfg);
        hotkeys::install_keymap(&app, key_map);
        auto tray_icon = tray_icon::create(&app);

//        auto keymap = new std::map<std::string, std::function<void()>>();
//        keymap->insert({"ctrl+meta+alt+O", window_management::position_lower_screen});
//        hotkeys::install_keymap(&a, keymap);

        return app.exec();
    } catch (const std::exception& e) {
        qDebug() << "Caught exception: " << e.what() << '\n';
    }
}
