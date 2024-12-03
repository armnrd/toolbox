#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <QPushButton>
#include <windows.h>
#include <QMessageBox>
#include "config.hpp"


int main(int argc, char *argv[])
{
    try {
        QApplication a(argc, argv);
        QPushButton button("Hello world!", nullptr);
        button.resize(200, 100);
        button.show();
//        Config config = ConfigHelper::load_config("config.yml");

        return QApplication::exec();
    } catch (const std::exception& e) {
        qDebug() << "Caught exception: " << e.what() << '\n';
    }
}
