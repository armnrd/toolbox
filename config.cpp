/**
 * @file config.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 11/11/2024
 */

#include "config.hpp"


// Function to load the YAML config file into a Config struct
Config *config::load_config(const std::string filename)
{
    return new YAML::Node(YAML::LoadFile(filename));
}

// Function to save the Config struct back into a YAML file
void config::save_config(const std::string filename, Config *cfg)
{
    std::ofstream fout(filename);
    if (fout.is_open()) {
        fout << *cfg;
        fout.close();
        qDebug() << std::format("YAML config written to {}.", filename);
    } else {
        qDebug() << std::format("Failed to open {} for writing YAML config to.", filename);
    }
}

hotkeys::KeyMap *config::keymap_from_config(Config *cfg)
{
    // Map from function names to functions
    static std::map<std::string, std::function<void()>> name_map = {
            {"window_management::action_minimize", window_management::action_minimize},
            {"window_management::action_toggle_maximize", window_management::action_toggle_maximize},
            {"window_management::action_close", window_management::action_close},
            {"window_management::action_resize", window_management::action_resize},
            {"window_management::position_centre", window_management::position_centre},
            {"window_management::position_lower_screen", window_management::position_lower_screen},
            {"window_management::position_upper_screen", window_management::position_upper_screen},
            {"window_management::position_right_middle", window_management::position_right_middle},
            {"window_management::position_top_right", window_management::position_top_right},
            {"window_management::position_top_middle", window_management::position_top_middle},
            {"window_management::position_top_left", window_management::position_top_left},
            {"window_management::position_left_middle", window_management::position_left_middle},
            {"window_management::position_bottom_left", window_management::position_bottom_left},
            {"window_management::position_bottom_middle", window_management::position_bottom_middle},
            {"window_management::position_bottom_right", window_management::position_bottom_right},
            {"window_management::move_fast_left", window_management::move_fast_left},
            {"window_management::move_slow_left", window_management::move_slow_left},
            {"window_management::move_fast_right", window_management::move_fast_right},
            {"window_management::move_slow_right", window_management::move_slow_right},
            {"window_management::move_fast_up", window_management::move_fast_up},
            {"window_management::move_slow_up", window_management::move_slow_up},
            {"window_management::move_fast_down", window_management::move_fast_down},
            {"window_management::move_slow_down", window_management::move_slow_down}
    };

    auto key_map = new hotkeys::KeyMap();

    // Use an iterator to iterate over the keys in the "hotkeys" section
    for (auto it = (*cfg)["hotkeys"]["window"].begin(); it != (*cfg)["hotkeys"]["window"].end(); ++it) {
        for (auto jt = it->second.begin(); jt != it->second.end(); ++jt) {
            auto function_name = std::format("window_management::{}_{}", it->first.as<std::string>(), jt->first.as<std::string>());
            // TODO error handling
            key_map->insert({jt->second.as<std::string>(), name_map[function_name]});
        }
    }

    return key_map;
}
