/**
 * @file toolbox.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 19/12/2024
 */

#ifndef TOOLBOX_TOOLBOX_HPP
#define TOOLBOX_TOOLBOX_HPP

#include <QApplication>


class Toolbox : public QApplication {
    // just holds information that needs to be passed around the various parts
public:
    Toolbox(int &argc, char **argv) : QApplication(argc, argv) {}
};

#endif //TOOLBOX_TOOLBOX_HPP
