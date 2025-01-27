/**
 * @file config.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 11/11/2024
 */

#ifndef TOOLBOX_CONFIG_HPP
#define TOOLBOX_CONFIG_HPP

#include <string>
#include <format>
#include <fstream>
#include <format>
#include <QDebug>
#include <yaml-cpp/yaml.h>


namespace config
{
    typedef YAML::Node Config;

    Config *config_from_file(const std::string filename);

    void config_to_file(Config *config, const std::string filename);

    void show_editor(Config *config);
};

#endif //TOOLBOX_CONFIG_HPP
