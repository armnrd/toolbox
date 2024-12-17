/**
 * @file tray_icon.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 17/12/2024
 */

#include "tray_icon.hpp"


QSystemTrayIcon *tray_icon::create(QApplication *app) {
    // TODO error handling
    // Check if the system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug() << "Error: System tray not available on this system.";
        return nullptr;
    }

    // Create a system tray icon
    QSystemTrayIcon *tray_icon = new QSystemTrayIcon();

    // Set the icon for the tray
    tray_icon->setIcon(app->style()->standardIcon(QStyle::SP_VistaShield));

    // Create a context menu for the tray icon
    QMenu *menu = new QMenu();

    // Add actions to the menu
    QAction *show_message_action = new QAction("Show Message", menu);
    QAction *quit_action = new QAction("Quit", menu);

    menu->addAction(show_message_action);
    menu->addSeparator();
    menu->addAction(quit_action);

    // Connect actions to slots
    QObject::connect(show_message_action, &QAction::triggered, []() {
        QMessageBox::information(nullptr, "Tray Message", "Hello from the system tray!");
    });

    QObject::connect(quit_action, &QAction::triggered, app, &QApplication::quit);

    // Set the context menu for the tray icon
    tray_icon->setContextMenu(menu);

    // Show the tray icon
    tray_icon->show();

    // Display a balloon message
    tray_icon->showMessage("Welcome", "Your app is running in the system tray!", QSystemTrayIcon::Information, 3000);

    return tray_icon;
}
