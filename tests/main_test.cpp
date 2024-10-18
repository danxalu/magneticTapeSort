#include <random>
#include <limits>
#include <sstream>
#include "Test.cpp"
#include "../src/TapeSorter.cpp"
#include "../include/Tape.h"

class MainTest : public Test {
public:
    MainTest(): Test(std::string("Рандомные входные данные"), [](){ TestTapes(); }) {}
    MainTest(std::string instanceName): Test(instanceName, [](){ TestTapes(); }) {}

    template <typename T>
    static std::vector<T> generateRandomVector(int count) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

        std::vector<int> numbers(count);
        for (int& num : numbers) {
            num = dis(gen);
        }
        return numbers;
    }

    static void TestTapes() {
        const std::string inputFileName = "input.bin";
        const std::string outputFileName = "output.bin";

        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        int numberOfElements = std::rand() % 100 + 1;

        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        int memoryLimitBytes = std::rand() % (numberOfElements+2) + 1;
        std::cout << "numbersOfElements: " << numberOfElements << "\nmemoryLimitBytes: " << memoryLimitBytes << std::endl;

        std::filesystem::remove(inputFileName);
        std::filesystem::remove(outputFileName);
        std::filesystem::remove_all("./tmp");

        std::ofstream file("input.bin", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Не удалось создать input-файл" << std::endl;
            return;
        }

        std::vector<int32_t> numbers = generateRandomVector<int32_t>(numberOfElements);
        
        file.write(reinterpret_cast<char*>(numbers.data()), sizeof(int32_t) * numbers.size());
        file.close();
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        int readDelay = std::rand() % 50 + 1;
        int writeDelay = std::rand() % 50 + 1;
        int shiftDelay = std::rand() % 50 + 1;
        int rewindDelay = std::rand() % 100 + 1;
        std::cout << "readDelay: " << readDelay << "\nwriteDelay: " << writeDelay << "\nshiftDelay: " << shiftDelay << "\nrewindDelay: " << rewindDelay << std::endl;

        FileTape inputTape(inputFileName, readDelay, writeDelay, shiftDelay, rewindDelay);
        FileTape outputTape(outputFileName, readDelay, writeDelay, shiftDelay, rewindDelay);

        std::string printInLine = inputTape.readAllEnd();
        std::cout << ">>> input: ||" << printInLine << "||" << std::endl;


        TapeSorter sorter(&inputTape, &outputTape, memoryLimitBytes);
        sorter.sort();

        outputTape.rewind();
        std::string printOutLine = outputTape.readAllEnd();
        std::cout << ">>> output: ||" << printOutLine << "||" << std::endl;
        outputTape.close();

        std::sort(numbers.begin(), numbers.end());
        std::ostringstream rightAnswer;
        for (int i = 0; i < numbers.size(); ++i) {
            rightAnswer << numbers[i];
            if (i < numbers.size() - 1) {
                rightAnswer << " ";
            }   
        }
        std::cout << ">>> right answer: ||" << rightAnswer.str() << "||" << std::endl;
        assert(rightAnswer.str() == printOutLine);
    }

};


int main() {
    std::vector<UnitTest*> tests;

    //Запускается 3 теста со случайными входными данными
    //Сравнение результата происходит с методом sort у всего входного вектора input-ленты
    for(size_t i = 1; i < 4; ++i){
        tests.push_back(new MainTest(std::to_string(i)));
    }

    RunTests(tests);

    for (UnitTest* test : tests) {
        delete test;
    }

    return 0;
}