2-Side-BFS based on a linear algebra
====================================
Реализация 2-side-BFS с ограничением по максимальной длине при помощи линейной алгебры и библиотеки __GraphBLAS__, __LAGraph__ и инструмент для измерения производительности алгоритма

Быстрый старт
-------------
Для сборки выполните:
```bash
sudo apt install -y g++ gcc cmake python3 &&
git clone https://github.com/georgiy-belyanin/2-side-bfs.git &&
cd 2-side-bfs &&
git submodule init &&
git submodule update &&
./scripts/get-graphblas.sh &&
export GRAPHBLAS_INCLUDE_DIR=`pwd`/graphblas-binaries/include &&
export GRAPHBLAS_LIBRARY=`pwd`/graphblas-binaries/lib/libgraphblas.so &&
mkdir build && cd build && cmake .. && make -j`nproc` && cd ..
```

Конвертация графа в нужный формат
---------------------------------
Для конвертации графов из формата csv в mtx используется python скрипт `scripts/convert-csv-to-mtx.py`:
```bash
python3 ./scripts/convert-csv-to-mtx.py <graph.csv> <graph.mtx> [dictionary.txt]
```
Словарь соответствия вершина/номер по-умолчанию называется `dictionary.txt`

Пример использования:
```bash
python3 ./scripts/convert-csv-to-mtx.py graph.csv graph.mtx
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

Для анализа результатов измерений можно воспользоваться __Jupyter-notebook__, графики доступны в `plots.ipynb`:
```bash
jupyter notebook results
```

Зависимости
-----------
Для сборки необходимы:
- `cmake`
- Компилятор `g++` или `clang++`
- Библиотека `SuiteSparse:GraphBLAS`

Для установки зависимостей сборки на debian-based дистрибутивах:
```bash
apt install -y g++ gcc cmake
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
Чтобы скачать собранную библиотеку можно воспользоваться одним из скриптов:
```bash
./scripts/get-graphblas.sh
```

Но при сборке библиотеки под конкретную машину время работы может стать заметно меньше: [установка SuiteSparse:GraphBLAS](https://github.com/DrTimothyAldenDavis/GraphBLAS/blob/stable/README.md).

Сборка
------
Для сборки проекта используется CMake
```bash
# Эти переменные нужны, если GraphBLAS скачивался pre-built
export GRAPHBLAS_INCLUDE_DIR=`pwd`/graphblas-binaries/include
export GRAPHBLAS_LIBRARY=`pwd`/graphblas-binaries/lib/libgraphblas.so
mkdir build && cd build && cmake .. && make -j`nproc` && cd ..
```

