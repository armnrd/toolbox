/**
 * @file config.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 11/11/2024
 */

#include "config.hpp"
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>


// Function to save the Config struct back into a YAML file
void save_config(const std::string &filename, const Config &cfg)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "General" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "AppName" << YAML::Value << cfg.app_name;
    out << YAML::Key << "Version" << YAML::Value << cfg.version;
    out << YAML::EndMap;
    out << YAML::Key << "Network" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Host" << YAML::Value << cfg.host;
    out << YAML::Key << "Port" << YAML::Value << cfg.port;
    out << YAML::EndMap;
    out << YAML::EndMap;

    std::ofstream fout(filename);
    fout << out.c_str();
}

// Function to load the YAML config file into a Config struct
Config load_config(const std::string &filename)
{
    YAML::Node config = YAML::LoadFile(filename);
    Config cfg;
    cfg.app_name = config["General"]["AppName"].as<std::string>();
    cfg.version = config["General"]["Version"].as<int>();
    cfg.host = config["Network"]["Host"].as<std::string>();
    cfg.port = config["Network"]["Port"].as<int>();
    return cfg;
}
