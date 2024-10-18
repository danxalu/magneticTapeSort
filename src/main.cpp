#include "TapeSorter.cpp"
#include "Config.cpp"
#include "../include/Tape.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Нет необходимых аргументов!" << std::endl << "Help: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    //Подключаем конфиги
    std::filesystem::path configPath = "../configs/config.txt";
    Config config = loadConfig(configPath);

    //Задаем входные данные
    const std::string inputFileName = argv[1];
    const std::string outputFileName = argv[2];
    const int memoryLimitBytes = config.memoryLimitBytes; 

    //Очищаем данные после прошлого запуска
    std::filesystem::remove(inputFileName);
    std::filesystem::remove(outputFileName);
    std::filesystem::remove_all("./tmp");

    //Генерируем input-файл
    std::ofstream file(inputFileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Не удалось создать input-файл" << std::endl;
        return 1;
    }

    //Записываем в input-файл нижеприведенные числа
    int32_t numbers[] = { 12345, -54321, 67890, -98765, 123456, -456789, 789012, -987654, 345678, -678901 };
    file.write(reinterpret_cast<char*>(numbers), sizeof(numbers));
    file.close();

    //Объявлением и инициализируем input- и output-ленты
    FileTape inputTape(inputFileName, config.readDelay, config.writeDelay, config.shiftDelay, config.rewindDelay);
    FileTape outputTape(outputFileName, config.readDelay, config.writeDelay, config.shiftDelay, config.rewindDelay);

    //Выводим данные с input-ленты
    std::string printInLine = inputTape.readAllEnd();
    std::cout << ">>> input: " << printInLine << std::endl;

    //Сортируем
    TapeSorter sorter(&inputTape, &outputTape, memoryLimitBytes);
    sorter.sort();

    //Выводим данные с output-ленты
    outputTape.rewind();
    std::string printOutLine = outputTape.readAllEnd();
    std::cout << ">>> output: " << printOutLine << std::endl;
    outputTape.close();

    return 0;
}