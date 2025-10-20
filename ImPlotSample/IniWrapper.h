#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <type_traits>

class IniWrapper {
public:
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return false;

        std::string line, section;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == ';') continue;
            if (line[0] == '[') {
                section = line.substr(1, line.find(']') - 1);
            }
            else {
                auto pos = line.find('=');
                if (pos == std::string::npos) continue;
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                data[section + "." + key] = value;
            }
        }
        return true;
    }

    template <typename T>
    T get(const std::string& section, const std::string& key, T def) const {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types are supported");

        auto it = data.find(section + "." + key);
        if (it == data.end()) return def;

        const std::string& raw = it->second;

        if (std::is_same<T, bool>::value) {
            std::string lower;
            std::transform(raw.begin(), raw.end(), std::back_inserter(lower), [](unsigned char c) {
                return std::tolower(c);
                });

            if (lower == "true" || lower == "1") return true;
            if (lower == "false" || lower == "0") return false;
            return def;
        }
        else {
            std::istringstream iss(raw);
            T value;
            return (iss >> value) ? value : def;
        }
    }

    template <typename T>
    void set(const std::string& section, const std::string& key, const T& value) {
        std::ostringstream oss;
        if (std::is_same<T, bool>::value) {
            oss << (value ? "true" : "false");
        }
        else {
            oss << value;
        }
        data[section + "." + key] = oss.str();
    }

    bool save(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) return false;

        std::vector<std::pair<std::string, std::string>> sorted_data(data.begin(), data.end());

        std::sort(sorted_data.begin(), sorted_data.end(),
            [](const std::pair<std::string, std::string>& a,
                const std::pair<std::string, std::string>& b) {
                    return a.first < b.first;
            });

        std::string last_section;
        for (const auto& entry : sorted_data) {
            const std::string& full_key = entry.first;
            const std::string& value = entry.second;

            auto dot = full_key.find('.');
            std::string section = full_key.substr(0, dot);
            std::string key = full_key.substr(dot + 1);

            if (section != last_section) {
                if (!last_section.empty()) file << "\n";
                file << "[" << section << "]\n";
                last_section = section;
            }
            file << key << "=" << value << "\n";
        }
        return true;
    }

private:
    std::unordered_map<std::string, std::string> data;
};
