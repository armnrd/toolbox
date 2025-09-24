#ifndef TOOLBOX_CONFIG_HPP
#define TOOLBOX_CONFIG_HPP

#include <QDebug>
#include <format>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

namespace toolbox::config
{
    typedef YAML::Node Config;

    Config *config_from_file(const std::string filename);

    void config_to_file(Config *config, const std::string filename);

    void show_editor(Config *config);
}; // namespace toolbox::config

#endif // TOOLBOX_CONFIG_HPP
