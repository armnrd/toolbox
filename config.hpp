/**
 * @file config.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 11/11/2024
 */

#ifndef TOOLBOX_CONFIG_HPP
#define TOOLBOX_CONFIG_HPP

#include "hotkeys.hpp"
#include "window_management.hpp"
#include <string>
#include <format>
#include <fstream>
#include <format>
#include <QDebug>
#include <yaml-cpp/yaml.h>


typedef YAML::Node Config;

namespace config
{
    Config *load_config(const std::string filename);

    void save_config(const std::string filename, Config *cfg);

    hotkeys::KeyMap *keymap_from_config(Config *cfg);
};

#endif //TOOLBOX_CONFIG_HPP
