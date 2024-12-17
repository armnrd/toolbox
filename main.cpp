#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <QPushButton>
#include <windows.h>
#include <QMessageBox>
#include <iostream>
#include "config.hpp"
#include "hotkeys.hpp"
#include "window_management.hpp"

void ctrl_meta_alt_O()
{
    qDebug() << "ctrl+meta+alt+O pressed.";
}

int main(int argc, char *argv[])
{
    try {
        QApplication a(argc, argv);
        QPushButton button("Hello world!", nullptr);
        button.resize(200, 100);
        button.show();

        auto cfg = config::load_config("config.yml");
        auto key_map = config::keymap_from_config(cfg);
        hotkeys::install_keymap(&a, key_map);

//        auto keymap = new std::map<std::string, std::function<void()>>();
//        keymap->insert({"ctrl+meta+alt+O", window_management::position_lower_screen});
//        hotkeys::install_keymap(&a, keymap);

        return QApplication::exec();
    } catch (const std::exception& e) {
        qDebug() << "Caught exception: " << e.what() << '\n';
    }
}
