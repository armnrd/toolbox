#ifndef TOOLBOX_TRAY_ICON_HPP
#define TOOLBOX_TRAY_ICON_HPP

#include "toolbox.hpp"
#include "config.hpp"
#include <QApplication>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QPainter>
#include <QIcon>
#include <QMessageBox>

namespace tray_icon
{
    class TrayIcon : public QSystemTrayIcon
    {
    Q_OBJECT

    public:
        TrayIcon(Toolbox *app, config::Config *config);

        ~TrayIcon() override;

    public slots:

    private slots:
        void show_config_editor();
//        void set_volume(int value);
//        void update_duration(qint64 duration);
//        void update_position(qint64 position);
//        void set_position(int position);
//        void update_status(QMediaPlayer::MediaStatus status);
//        void update_ticker();
//        void load_file(const QString &file_path);

    private:

        Toolbox *app;
        config::Config *config;
    };
}

#endif //TOOLBOX_TRAY_ICON_HPP
