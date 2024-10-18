# magneticTapeSort
Программа принимает два бинарных файла в качестве входных данных, сортирует данные и записывает отсортированные данные в другой файл.

## Требования
- Компилятор C++ с поддержкой стандарта C++17

## Установка

1. Склонируйте репозиторий 

2. Откройте в VS Code директорию проекта

3. Соберите проект:
   Terminal -> Run Task -> "C/C++: g++ сборка активного файла"
   ! Возможны проблемы с настройкой компилятора g++ на вашем компьютере, устанавливать и настраивать его -- дело отдельное.

## Использование

1. Откройте конфигурационный файл configs/config.txt, и впишите необходимые значения параметров:
   M -- ограничение в байтах на использование оперативной памяти сортировки входной ленты
   readDelay -- задержка чтения в миллисекундах
   writeDelay -- задержка записи в миллисекундах
   rewindDelay -- задержка перемотки в миллисекундах
   shiftDelay -- задержка сдвига в миллисекундах

2. Запустите программу со следующей командой из директории build:
   ./main <путь-до-входного-файла> <путь-до-выходного-файла>

   В реализации main.cpp очищаются существующие входные и выходные файлы и создается новый входной файл.
   ! Вы можете закоментировать эти строки кода и подкладывать собственный input-файл, 
   однако учитывайте, что файлы чиаются в бинарном режиме.

   Во входной файл бинарным форматом записываются числа из вектора numbers.

   Содержание входного файла печатаются в консоли.

   Дальше происходит механизм сортировки.

   Результат сортировки бинарным форматом записывается в выходной файл.

   Содержание этого файла печатается в консоли.


## Конфигурация

Конфигурация программы хранится в файле `config.txt`. Вы можете изменить следующие параметры:

- `input_file_path`: Путь к входному бинарному файлу.
- `output_file_path`: Путь к выходному бинарному файлу.

## Тестирование

Запустите 3 теста одинаковой логики со случайными входными данными:
   Terminal -> Run Task -> "С/C++: Запустить тесты"

   Далее скрипт скопилирует приложение и запустит его.
   В консоль будет воспроизводиться процесс проверки правильности результатов данного алгоритма сортировки элементов ленты, основанный на проверке с помощью сортировки std::sort всей входной ленты. 