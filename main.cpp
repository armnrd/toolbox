#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <QPushButton>
#include <windows.h>
#include <QMessageBox>
#include <iostream>
#include "config.hpp"


int main(int argc, char *argv[])
{
    try {
        auto cfg = ConfigHelper::load_config("config.yml");
        std::cout << cfg["hotkeys"]["window"]["position"]["right_middle"].as<std::string>();
        QApplication a(argc, argv);
        QPushButton button("Hello world!", nullptr);
        button.resize(200, 100);
        button.show();

        return QApplication::exec();
    } catch (const std::exception& e) {
        qDebug() << "Caught exception: " << e.what() << '\n';
    }
}
