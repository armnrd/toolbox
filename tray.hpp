#ifndef TOOLBOX_TRAY_ICON_HPP
#define TOOLBOX_TRAY_ICON_HPP

#include "config.hpp"
#include <QApplication>
#include <QSystemTrayIcon>

namespace toolbox::tray
{
    class TrayIcon : public QSystemTrayIcon
    {
    Q_OBJECT

    public:
        TrayIcon(QApplication *app, config::Config *config);

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
        QApplication *app;
        config::Config *config;
    };

    class AppTrayIcon : public QSystemTrayIcon
    {
        Q_OBJECT

        public:
        AppTrayIcon(HWND app_hwnd, QIcon app_icon);

        ~AppTrayIcon() = default;

    public slots:
        void toggle_app_window();
        void hide_app_window();
        void unhide_app_window();

    private:
        HWND app_hwnd;
        QIcon app_icon;
    };

    AppTrayIcon* create_app_tray_icon(HWND app_hwnd);
}

#endif //TOOLBOX_TRAY_ICON_HPP
