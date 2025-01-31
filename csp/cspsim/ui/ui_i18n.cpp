#include <fstream>
#include <nlohmann/json.hpp>

#include "ui.h"

static std::string currentLanguage = "en";

static std::unordered_map<std::string, std::string> l_translations;

void ui_i18n_set_language(const std::string &language) {
	l_translations = ui_i18n_load(language);
}

const char* ui_i18n_get(const char *key) {
    if (l_translations.count(key)) {
        return l_translations[key].c_str();
    }
    return key;
}

std::unordered_map<std::string, std::string> ui_i18n_load(const std::string& language) {
	std::string filename = "data/ui/i18n/" + language + ".json";
    std::ifstream file(filename);
    nlohmann::json jsonData;
    file >> jsonData;

	std::unordered_map<std::string, std::string> translations;
	for (const auto& [key, value] : jsonData.items()) {
		translations[key] = value;
	}
	return translations;
}
