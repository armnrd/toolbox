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


// Structure to hold the configuration data
struct Config
{
    std::string app_name;
    int version;
    std::string host;
    int port;

};

enum struct ConfigEnum
{
    HOTKEYS_CLOSE
};

class ConfigHelper
{
public:
    static Config load_config(const std::string& filename);

    static void save_config(const std::string& filename, const Config& cfg);

private:
    static ConfigEnum string_to_enum(std::string string);
};


#endif //TOOLBOX_CONFIG_HPP
