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
    QPixmap pixmap(256, 256); // Adjust size as needed for the tray icon
    pixmap.fill(Qt::transparent);

    // Draw the emoji onto the pixmap
    QPainter painter(&pixmap);
    QFont font("Segoe UI Emoji", 160); // Font that supports emojis
    painter.setFont(font);
    painter.setPen(Qt::black); // Set pen color for the emoji
    painter.drawText(pixmap.rect(), Qt::AlignCenter, emoji); // Center the emoji
    painter.end();

    return QIcon(pixmap);
}

// TODO Toolbox object holds members that the various parts need to access
tray_icon::TrayIcon::TrayIcon(Toolbox *app, config::Config *config) : QSystemTrayIcon(app)
{
    // TODO error handling
    // Check if the system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug() << "Error: System tray not available on this system.";
        return;
    }

    // Store reference to Toolbox and Config objects for later
    this->app = app;
    this->config = config;

    // Set the icon
    setIcon(icon_from_emoji("🛠"));

    // Create a context menu for the tray icon
    auto menu = new QMenu();

    // Add actions to the menu
    QAction *menu_settings = new QAction("Settings...", menu);
    QAction *menu_quit = new QAction("Quit", menu);

    menu->addAction(menu_settings);
    menu->addSeparator();
    menu->addAction(menu_quit);

    // Connect actions to slots
    connect(menu_settings, &QAction::triggered, this, &TrayIcon::show_config_editor);
    connect(menu_quit, &QAction::triggered, this, &QApplication::quit);

    // Set the context menu for the tray icon
    setContextMenu(menu);

    // Show the tray icon
    show();

    // Display a balloon message
    showMessage("Toolbox", "Running in the system tray.", QSystemTrayIcon::Information, 2000);
}

tray_icon::TrayIcon::~TrayIcon()
{
    delete app;
}

void tray_icon::TrayIcon::show_config_editor()
{
    config::show_editor(config);
    // use config::load_config()
}
