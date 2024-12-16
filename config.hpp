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
#include <yaml-cpp/yaml.h>


typedef YAML::Node Config;

namespace ConfigHelper
{
    Config load_config(const std::string& filename);

    void save_config(const std::string& filename, const Config& cfg);
};

#endif //TOOLBOX_CONFIG_HPP
