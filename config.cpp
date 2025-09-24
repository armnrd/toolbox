#include "config.hpp"

namespace toolbox::config
{
    // Function to load the JSON config file into the Toolbox object
    Config *config_from_file(const std::string filename)
    {
        // TODO error handling
        qDebug() << "Loading config from file:" << QString::fromStdString(filename);
        return new Config(YAML::LoadFile(filename));
    }

    // Function to save the Config struct inside the Toolbox object back into a YAML file
    void config_to_file(Config *config, const std::string filename)
    {
        std::ofstream fout(filename);
        if (fout.is_open())
        {
            fout << *config;
            fout.close();
            qDebug() << std::format("YAML config written to {}.", filename);
        } else
        {
            qDebug() << std::format("Failed to open {} for writing YAML config to.", filename);
        }
    }

    void show_editor(Config *config)
    {
        // TODO show editor
    }
} // namespace toolbox::config
