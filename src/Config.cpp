#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <map>

struct Config {
    int32_t memoryLimitBytes, readDelay, writeDelay, rewindDelay, shiftDelay;
};

//Функция подгрузки конфигураций из файла
Config loadConfig(const std::filesystem::path& configPath) {
    Config config;

    if (std::filesystem::exists(configPath)) {
        std::ifstream configFile(configPath);
        std::string line;

        while (std::getline(configFile, line)) {
            std::istringstream curLine(line);
            std::string key, value;
            std::getline(curLine, key, '=');
            std::getline(curLine, value);
            if (key == "M") {
                config.memoryLimitBytes = std::stoi(value);
            } else if (key == "readDelay") {
                config.readDelay = std::stoi(value);
            } else if (key == "writeDelay") {
                config.writeDelay = std::stoi(value);
            } else if (key == "rewindDelay") {
                config.rewindDelay = std::stoi(value);
            } else if (key == "shiftDelay") {
                config.shiftDelay = std::stoi(value);
            }
        }
    } else {
        std::cerr << "Конфигурационный файл " << configPath << " не найден" << std::endl;
    }

    return config;
}