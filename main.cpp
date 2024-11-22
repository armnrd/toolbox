#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include "config.hpp"


int main(int argc, char *argv[])
{
    try {
        QApplication a(argc, argv);
        QPushButton button("Hello world!", nullptr);
        button.resize(200, 100);
        button.show();
        Config config = load_config("config.yml");

        return QApplication::exec();
    } catch (const std::exception& e) {
        qDebug() << "Caught exception: " << e.what() << '\n';
    }
}
