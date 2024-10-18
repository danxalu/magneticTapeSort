#include "Tape.cpp"
#include <algorithm>
#include <vector>
#include <filesystem>
#include <queue>

class TapeSorter {
public:
    TapeSorter(FileTape* inputFile, FileTape* outputFile, const int32_t memoryLimit)
        : input(inputFile), output(outputFile), memoryLimit(memoryLimit) {
    }

    //Сортируем элементы из буфера и вставляем их в output-ленту
    void sortWriteBuffer(std::vector<int32_t>& buffer, size_t size, FileTape* output) {
        std::sort(buffer.begin(), buffer.begin() + size);
        for (size_t i = 0; i < size; ++i) {
            output->write(buffer[i]);
        }
    }

    //Метод записи новой temp-ленты в min-кучу Sources пар значений (первый элемент на temp-ленте; индекс temp-ленты для ее идентификации)
    //Принимает min-кучу Sources temp-лент, буфер и размер буфера
    void writeSources(std::priority_queue<std::pair<int32_t, size_t>, std::vector<std::pair<int32_t, size_t>>, std::greater<std::pair<int32_t, size_t>>>& sources,
                    std::vector<int32_t>& buffer, size_t size) {
        for (size_t i = 0; i < size; ++i) {
            sources.push({buffer[i], i});
        }
    }

    //Метод сортировки лент
    void sort() {
        //Cоздаем директорию tmp (она точно отсутствует, так как мы ее удаляли до этого)
        std::filesystem::create_directory("tmp");
        //Вектор временных лент
        std::vector<FileTape> tempTapes;

        //Ревайндим input-ленту и output-ленту
        input->rewind();
        output->rewind();

        //Считаем размер буфера, учитывая, что элементы лент по условию имеют тип int32_t
        size_t bufferSize = memoryLimit / sizeof(int32_t);
        //Объявляем буфер с этим размером
        std::vector<int32_t> buffer(bufferSize);

        while (true) {
            size_t readIndex = 0;

            //Заполняем буфер
            for (size_t i = 0; i < bufferSize; ++i) {
                //Читаем ленту, делаем сдвиг вперед
                int32_t value = input->read();
                input->moveForward();
                //Если достигли конца input-ленты -- дальше не идем, иначе заполняем буфер элементом, вычитанным из input-ленты
                if (input->eof()) break;
                buffer[i] = value;
                //readIndex ведет статистику количества элементов в текущем буфере
                readIndex++;
            }

            //Если буфер пуст -- дальше не идем
            if (readIndex == 0) break;

            //Сортируем буфер
            std::sort(buffer.begin(), buffer.begin() + readIndex);

            //Записываем результат сортировки в temp-ленту
            FileTape temp("./tmp/temp_" + std::to_string(tempTapes.size()) + ".bin");
            tempTapes.emplace_back(std::move(temp));
            for (size_t i = 0; i < readIndex; ++i) {
                tempTapes.back().write(buffer[i]);
            }
        }
        //Закрываем поток файла input-ленты
        input->close();

        //Если вдруг temp-лент нет 
        //(temp-ленты должны быть в любом случае, это просто доплогика, в данной доплогике количество элементов input-ленты обязано влезать в буфер)
        if (tempTapes.empty()) {
            size_t bufferIndex = 0;
            std::vector<int32_t> buffer(bufferSize);
            
            //Считываем все элементы с input-ленты, добавляем их в буфер и сортируем их в нем
            while (!input->eof()) {
                buffer[bufferIndex] = input->read();
                input->moveForward();
                bufferIndex++;
                //Если буфер заполнился -- сортируем и добавляем его элементы в output-ленту
                if (bufferIndex == bufferSize) {
                    sortWriteBuffer(buffer, bufferSize, output);
                    bufferIndex = 0;
                }
            }
            //Если оставшийся буфер не пустой (он будет явно не заполнен до конца) -- сортируем и добавляем его элементы в min-кучу temp-лент
            if (bufferIndex > 0) {
                sortWriteBuffer(buffer, bufferSize, output);
            }
        } else { //Если temp-ленты есть
            std::priority_queue<std::pair<int32_t, size_t>, std::vector<std::pair<int32_t, size_t>>, std::greater<std::pair<int32_t, size_t>>> sources;
            std::vector<int32_t> buffer(bufferSize);
            size_t bufferIndex = 0;

            //Перебираем все temp-ленты
            for (FileTape& tape : tempTapes) {
                tape.rewind();
                while (!tape.eof()) {
                    //Считываем элемент текущей temp-ленты и двигаемся магнитной головкой вперед
                    int32_t value = tape.read();
                    tape.moveForward();
                    //Если лента закончилась -- дальше не идем
                    if (tape.eof()) {
                        break;
                    }
                    //Записываем в буфер вычитанный элемент
                    buffer[bufferIndex] = value;
                    bufferIndex++;
                    //Если буфер заполнился -- добавляем его уже отсортированные элементы в min-кучу temp-лент
                    if (bufferIndex == bufferSize) {
                        writeSources(sources, buffer, bufferIndex);
                        bufferIndex = 0;
                    }
                    
                }
                //Если оставшийся буфер не пустой (он будет явно не заполнен до конца) -- добавляем его уже отсортированные элементы в min-кучу temp-лент
                if (bufferIndex > 0) {
                    writeSources(sources, buffer, bufferIndex);
                }
            }

            //Записываем результаты сортировки с temp-лент в output-ленту (помним, что Sources -- min-куча по первому элементу temp-лент)
            while (!sources.empty()) {
                output->write(sources.top().first);
                sources.pop();
            }
        }

        //Закрываем потоки файлов temp-лент (опционально потом подчищаем их файлы)
        for (FileTape& tape : tempTapes) {
            tape.close();
            //std::filesystem::remove(tape.getfileName());
        }

    }

private:
    FileTape* input;
    FileTape* output;
    const int32_t memoryLimit;
};