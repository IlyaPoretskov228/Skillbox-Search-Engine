cmake_minimum_required(VERSION 3.15)
project(search_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# Опция сборки тестов
option(BUILD_TESTS "Build tests" ON)

# ВАЖНО: ищем библиотеку потоков
find_package(Threads REQUIRED)

# Подпроект библиотеки (lib/)
add_subdirectory(lib)

add_subdirectory(src)

# Подключаем подпроект tests/
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()