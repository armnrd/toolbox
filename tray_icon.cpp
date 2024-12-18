/**
 * @file tray_icon.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 17/12/2024
 */

#include "tray_icon.hpp"


// Function to create an emoji-based QIcon
QIcon icon_from_emoji(const QString &emoji) {
    // Create a pixmap with a transparent background
    QPixmap pixmap(32, 32); // Adjust size as needed for the tray icon
    pixmap.fill(Qt::transparent);

    // Draw the emoji onto the pixmap
    QPainter painter(&pixmap);
    QFont font("Segoe UI Emoji", 24); // Font that supports emojis
    painter.setFont(font);
    painter.setPen(Qt::black); // Set pen color for the emoji
    painter.drawText(pixmap.rect(), Qt::AlignCenter, emoji); // Center the emoji
    painter.end();

    return QIcon(pixmap);
}

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
    tray_icon->setIcon(icon_from_emoji("🛠"));

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
    tray_icon->showMessage("Toolbox", "Running in the system tray.", QSystemTrayIcon::Information, 2000);

    return tray_icon;
}
