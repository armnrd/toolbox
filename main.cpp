/**
 * @file main.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 19/12/2024
 */

#include "toolbox.hpp"
#include "config.hpp"
#include "hotkeys.hpp"
#include "tray_icon.hpp"
#include "hacks.hpp"

int main(int argc, char *argv[])
{
//    hacks::set_display_scaling(125, 0);
    try {
        auto app = new Toolbox(argc, argv);

        auto config = config::config_from_file("config.yml");

        auto hotkey_factory = new hotkeys::Factory();
        hotkey_factory->set_keymap(hotkeys::keymap_from_config(config), app);

        auto tray_icon = new tray_icon::TrayIcon(app, config);

        return app->exec();
    } catch (const std::exception& e) {
        qDebug() << "Caught exception: " << e.what() << '\n';
    }
}
