#include "Config.h"
#include "Platform.h"

/**
 * Global configuration file
 */
SimpleConfig g_Config;


bool openConfig(std::string ini_path, bool report_error) {
        ini_path = ospath::filter(ini_path);
        bool found_config = false;
        try {
                found_config = g_Config.open(ini_path);
        } catch (ConfigError &e) {
                return false;
        }
        if (!found_config && report_error) {
                std::cerr << "Unable to open configuration file " << ini_path << std::endl;
        }
        return found_config;
}

