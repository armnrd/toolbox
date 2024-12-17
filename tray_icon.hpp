/**
 * @file tray_icon.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 17/12/2024
 */

#ifndef TOOLBOX_TRAY_ICON_HPP
#define TOOLBOX_TRAY_ICON_HPP

#include <QApplication>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QMessageBox>


namespace tray_icon
{
    QSystemTrayIcon *create(QApplication *app);
}

#endif //TOOLBOX_TRAY_ICON_HPP
