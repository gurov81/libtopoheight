libtopoheight
=============

Обработка геопространственных данных для получения информации о рельефе местности.

Установка зависимостей
----------------------

Для Ubuntu Linux:

```bash
sudo apt install build-essential cmake luajit
```

Сборка и проверка
-----------------

```bash
mkdir build
cd build
cmake ..
make
```

В процессе сборки автоматически вызываются unit-тесты (см. скрипт-файл ``tests/run.lua`` и тесты ``tests/test_*.lua``)

Пример вывода результатов:
```
sun@gurov:~/projects/heightmap/libtopoheight/build$ cmake ..
-- The C compiler identification is GNU 7.5.0
-- The CXX compiler identification is GNU 7.5.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/sun/projects/heightmap/libtopoheight/build
sun@gurov:~/projects/heightmap/libtopoheight/build$ make -j
Scanning dependencies of target topoheight
[ 40%] Building C object CMakeFiles/topoheight.dir/third_party/rtree.c/rtree.c.o
[ 40%] Building CXX object CMakeFiles/topoheight.dir/source/libtopoheight.cpp.o
[ 60%] Building CXX object CMakeFiles/topoheight.dir/source/picture.cpp.o
[ 80%] Linking CXX shared library topoheight.so
[ 80%] Built target topoheight
Scanning dependencies of target topoheight-test
[100%] testing libtopoheight
Started on Mon May 16 12:11:09 2022
    TestHeight.testCase_0.5_0 ... Ok
    TestHeight.testCase_0.5_1 ... Ok
    TestHeight.testCase_0_0.5 ... Ok
    TestHeight.testCase_1_0.5 ... Ok
    TestHeight.testInPoints_0_0 ... Ok
    TestHeight.testInPoints_0_1 ... Ok
    TestHeight.testInPoints_1_0 ... Ok
    TestHeight.testInPoints_1_1 ... Ok
    TestHeightProperty.test1 ... Ok
    TestHeightmap.testPolygon_WithColorCallback ... Ok
    TestHeightmap.testRead_LAYER6 ... Ok
    TestLoadBuffer.testMalformed_JSON ... Cannot parse JSON
Ok
    TestLoadBuffer.testMultiPoint_Doubles ... Ok
    TestLoadBuffer.testPolygon_Doubles_3points ... Ok
    TestLoadBuffer.testPolygon_Doubles_4points ... Ok
    TestLoadBuffer.testPolygon_Doubles_5points ... Ok
    TestLoadBuffer.testPolygon_Integers ... Ok
    TestLoadFile.testRead_LAYER13 ... Ok
    TestLoadFile.testRead_LAYER6 ... Ok
=========================================================
Ran 19 tests in 2.690 seconds, 19 successes, 0 failures
OK
[100%] Built target topoheight-test
```

Краткое описание
----------------

Для сборки используется система cmake.

В результате сборки исходных текстов из каталога ``source`` создается библиотека ``build/topoheight.so``.

Библиотека экспортирует API в виде функций с интерфейсом в стиле ANSI C.
Реализация выполнена на C++ в модуле ``source/libtopoheight.cpp``.

Использование интерфейса к библиотеке в стиле "С" позволяет динамически загружать и вызывать функции библиотеки
из других языков, к примеру C/Python/Lua/Javascript (а не только из C++).

В каталоге ``third_party`` расположены текущие зависимости библиотеки:
- ``third_party/rapidjson`` - реализация разбора формата JSON (в виде залоговочных файлов)
- ``third_party/delaunator-cpp`` - реализация алгоритма триангуляции Делоне (в виде заголовочных файлов), см.https://github.com/delfrrr/delaunator-cpp
- ``third_party/rtree.c`` - реализация работы с r-деревом (https://github.com/tidwall/rtree.c)

Тесты расположены в каталоге ``tests`` и используют трассирующий компилятор luajit:
- ``tests/libtopoheight.lua`` содержит FFI-обертку на C API библиотеки для вызова функций из библиотеки в скриптах luajit
- ``tests/run.lua`` - основной исполняемый файл, осуществляющий подключение и выполнение тестов
- ``tests/test_*.lua`` - набор тестов
- ``tests/helpers.lua`` - дополнительные функции, используемые при тестировании
- ``tests/lib/dkjson.lua`` - генератор строки в формате JSON
- ``tests/lib/luaunit.lua`` - основной движок тестирования
- ``tests/examples/*.geojson`` - примеры GeoJSON-файлов

Объем и логика тестирования
---------------------------

1. Проверка разбора данных в формате GeoJSON (как корректных, так и некорректных).

2. Проверка промежуточных результатов выполнения преобразования данных.

3. Проверку правильности выполнения расчета высот на синтетических примерах.

Т.к. в данный момент не весь функционал реализован, соответствующие тесты завершаются с ошибкой.
Качественной оценкой правильности реализации библиотеки является то, что тесты, не срабатывающие в исходной версии,
начинают выполняться без ошибок (т.е. подход к разработке, похожий на Test Driven Development).

Credits
-------

Используются (см.third_party) наработки (модифицированные) из репозиториев:
- https://github.com/delfrrr/delaunator-cpp
- https://github.com/tidwall/rtree.c
