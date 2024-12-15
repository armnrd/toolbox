/**
 * @file config.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 11/11/2024
 */

#include "config.hpp"
#include <fstream>
#include <format>
#include <QDebug>
#include <yaml-cpp/yaml.h>


// Function to save the Config struct back into a YAML file
void ConfigHelper::save_config(const std::string &filename, const Config &cfg)
{
    std::ofstream fout(filename);
    if (fout.is_open()) {
        fout << cfg;
        fout.close();
        qDebug() << std::format("YAML config written to {}.", filename);
    } else {
        qDebug() << std::format("Failed to open {} for writing YAML config to.", filename);
    }
}

// Function to load the YAML config file into a Config struct
Config ConfigHelper::load_config(const std::string &filename)
{
    return YAML::LoadFile(filename);
}
