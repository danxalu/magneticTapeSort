#pragma once
#include "../include/Tape.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

class FileTape : public Tape {
public:
    //Инициализируем ленту
    FileTape(const std::string& fileName, int32_t readDelay = 10, int32_t writeDelay = 10, int32_t rewindDelay = 100, int32_t shiftDelay = 10)
        : fileName(fileName), readDelay(readDelay), writeDelay(writeDelay), shiftDelay(shiftDelay), rewindDelay(rewindDelay) {
        //Если файл ленты не существует -- создаем его, finally открываем файл
        if (!std::filesystem::exists(fileName)){
            std::ofstream newFile(fileName, std::ios::binary);
            newFile.open(fileName, std::ios::binary | std::ios::in | std::ios::out);
            if (newFile.is_open()) {
                newFile.close();
            } else {
                std::cerr << "Ошибка открытия файла " << fileName << " для I/O" << std::endl;
                return;
            }
        }
        file.open(fileName, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open() || !file.good()) {
            std::cerr << "Ошибка открытия файла " << fileName << std::endl;
            return;
        }

    }

    //Перематываем ленту в начало для потоков чтения и записи
    void rewind() override {
        file.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(rewindDelay));
        file.seekg(0, std::ios::beg);
        file.seekp(0, std::ios::beg);
    }

    //Методы сдвига ленты вперед-назад через метод move
    void moveForward() override {
        move(true, true);
    }

    void moveBackward() override {
        move(false, true);
    }

    //Записываем в данной позиции элемент
    void write(const int32_t& value) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(writeDelay));
        file.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
        file.flush();
        
    }

    //Читвем в данной позиции элемент и остаемся в данной позиции после чтения
    int32_t read() override {
        int32_t value;
        std::this_thread::sleep_for(std::chrono::milliseconds(readDelay));
        file.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
        move(false, false);
        return value;
    }

    //Метод для считывания всей ленты (вспомогательный метод для отладки)
    std::string readAllEnd() {
        std::string printLine;
        std::streampos curPos = file.tellg();
        int32_t value;
        while (file.read(reinterpret_cast<char*>(&value), sizeof(int32_t))) {
            printLine += std::to_string(value);
            if (file.peek() != EOF){
                printLine += " ";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(readDelay));
        }
        file.seekg(curPos); 
        return printLine;
    }  

    //Метод, возвращающий название файла ленты
    std::string getfileName() const override {
        return fileName;
    }

    //Проверяет файл ленты на eof
    bool eof() {
        return file.eof();
    }

    //Открываем потоки файла
    void open() {
        file.open(fileName, std::ios::binary | std::ios::in | std::ios::out);
    }

    //Закрываем файл
    void close() {
        file.close();
    }

private:
    std::fstream file;
    std::string fileName;
    int32_t readDelay, writeDelay, rewindDelay, shiftDelay;

    //Метод для сдвига магнитной головки
    void move(bool forward, bool delay){
        if (file.eof()) {
            return;
        }

        if (file.fail()) {
            std::cerr << "Ошибка в потоке перед перемещением" << std::endl;
            return;
        }
        
        std::streampos currentPos = file.tellg();
        if (currentPos < 0) {
            std::cerr << "Ошибка: текущая позиция недоступна" << std::endl;
            return;
        }
        
        if (delay) {
            std::this_thread::sleep_for(std::chrono::milliseconds(shiftDelay));
        }
        
        //В зависимости от своих целей -- двигаемся вперед-назад
        currentPos += (forward ? 1 : -1) * sizeof(int32_t);
            
        file.seekg(currentPos);
        if (file.fail()) {
            std::cerr << "Не удалось переместить позицию" << std::endl;
            return;
        }

        currentPos = file.tellg();

        if (currentPos < 0) {
            std::cerr << "Ошибка, текущая позиция недоступна" << std::endl;
        }
    }
};