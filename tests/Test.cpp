#pragma once
#include "../include/UnitTest.h"
#include <iostream>
#include <vector>
#include <functional>
#include <string>

class Test : public UnitTest {
public:
    Test(std::string name, std::function<void()> testFunc): name(name), testFunc(testFunc) {}

    void run() override {
        testFunc();
    }

    std::string getTestName() const override {
        return name;
    }

private:
    std::string name;
    std::function<void()> testFunc;
};


void RunTests(std::vector<UnitTest*>& tests) {
    for (UnitTest* test : tests) {
        std::cout << "\nЗапускается тест: " << test->getTestName() << std::endl;
        try {
            test->run();
            std::cout << "Тест " << test->getTestName() << " пройден" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Тест " << test->getTestName() << " провалился: " << e.what() << std::endl;
        }
    }
}