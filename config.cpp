/**
 * @file config.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 11/11/2024
 */

#include "config.hpp"
#include "toolbox.hpp"


// Function to load the JSON config file into the Toolbox object
config::Config *config::config_from_file(const std::string filename)
{
    // TODO error handling
    return new Config(Config::parse(std::ifstream(filename)));
}

// Function to save the Config struct inside the Toolbox object back into a JSON file
void config::config_to_file(config::Config *config, const std::string filename)
{
    std::ofstream fout(filename);
    if (fout.is_open()) {
        fout << config->dump(4);
        fout.close();
        qDebug() << std::format("JSON config written to {}.", filename);
    } else {
        qDebug() << std::format("Failed to open {} for writing JSON config to.", filename);
    }
}

void config::show_editor(config::Config *config)
{
    // TODO show editor
}