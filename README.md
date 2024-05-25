Linear algebra based 2-side-BFS
===============================
Реализация 2-side-BFS с ограничением по максимальной длине при помощи линейной алгебры и библиотек __SuiteSparse:GraphBLAS__, __LAGraph__ и инструмент для измерения производительности алгоритма

Быстрый старт
-------------
Для сборки выполните:
```bash
sudo apt install -y g++ gcc cmake python3 jupyter-notebook libsuitesparse-dev &&
git clone https://github.com/georgiy-belyanin/2-side-bfs.git &&
cd 2-side-bfs &&
git submodule update --init --recursive &&
mkdir build &&
cmake -S . -B build/ &&
cmake --build build
```
Проверено на Ubuntu 22.04, gcc 12.3.0

Конвертация графа в нужный формат
---------------------------------
Для конвертации графов из формата csv в mtx используется python скрипт `scripts/convert-csv-to-mtx.py`:
```bash
python3 ./scripts/convert-csv-to-mtx.py <graph.csv> <graph.mtx> [dictionary.txt]
```
Словарь соответствия вершина/номер по-умолчанию называется `dictionary.txt`

Пример использования:
```bash
python3 ./scripts/convert-csv-to-mtx.py examples/graph.csv graph.mtx
```

Запуск бенчмарков
-----------------
Для измерения производительности в зависимости от длины можно использовать следующую команду:
```bash
./build/two-side-bfs
    [-r число запусков для каждой длины (по-умолчанию: 1000)]
    [-l наименьшая изучаемая длина пути (по-умолчанию: 2)]
    [-m наибольшая изучаемая длина пути (по-умолчанию: 5)]
    <graph.mtx>
    [results/]
```
Для каждой из максимальной длин в диапазоне задаваемом аргументами `l` и `m` будет получен файл по пути `results/<макс-длина-пути>.txt`, в файлах построчно содержится время запуска кажддого из запросов.

Пример использования:
```bash
./build/two-side-bfs graph.mtx
```
В консоль для каждой макс. длины при этом будет выведена информация о среднем времени работы, стандартном отклонении, проценте запросов, в которых путь нашёлся и число запросов в секунду

Для детального анализа результатов измерений можно воспользоваться __Jupyter-notebook__, нужные скрипты находятся в файле `plots.ipynb`:
```bash
jupyter notebook <results/>
```
После выполнения команды откроется браузер, в jupyter notebook необходимо выполнить каждую ячейку, сделать это можно выбором `Cell > Run All` в тулбаре, или многократным нажатием `Shift+Enter`. Отрисованные boxplot появятся внизу файла и станут доступны в директории с результатами с названиями `both.png` - boxplot для всех случаев, `path-exists.png` - когда путь есть, `not-exists.png` - когда пути нет. В Boxplot чёрным выделяется медиана, зелёным - среднее.

Зависимости
-----------
Для сборки необходимы:
- `cmake`
- Компилятор `g++` или `clang++`
- Библиотека `SuiteSparse:GraphBLAS`

Для установки зависимостей сборки на debian-based дистрибутивах:
```bash
sudo apt install -y g++ gcc cmake python3 jupyter-notebook libsuitesparse-dev
```

Для сборки так же необходимо получить сабмодули:
```bash
git submodule init
git submodule update
```

Дополнительно для запуска некоторых скриптов нужен __Python 3__
```
apt install -y python3
```

SuiteSparse:GraphBLAS
---------------------
SuiteSparse:GraphBLAS доступен в репозиториях популярных дистрибутивов
```bash
./scripts/get-graphblas.sh
```

Но при сборке библиотеки под конкретную машину время работы может стать заметно меньше: [установка SuiteSparse:GraphBLAS](https://github.com/DrTimothyAldenDavis/GraphBLAS/blob/stable/README.md).

Сборка
------
Для сборки проекта используется CMake необходимо выполнить из корня репозитория:
```bash
# Эти переменные нужны, если GraphBLAS скачивается pre-built
mkdir build && cd build && cmake .. -DGRAPHBLAS_INCLUDE_DIR=${GRAPHBLAS_INCLUDE_DIR} -DGRAPHBLAS_LIBRARY=${GRAPHBLAS_LIBRARY} && make -j`nproc` && cd ..
```

