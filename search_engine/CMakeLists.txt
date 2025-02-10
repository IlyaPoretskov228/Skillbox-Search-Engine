cmake_minimum_required(VERSION 3.15)
project(search_engine)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Устанавливаем политики для подавления предупреждений
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW) # option() honors normal variables
endif()

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW) # Proper support for visibility in object libraries
endif()

# Опция для сборки тестов (по умолчанию ON)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_compile_definitions(UNIT_TEST)
endif()

# Подключаем Google Test через FetchContent
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
# Отключение pthreads для MinGW
set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
# Принудительное использование совместной CRT библиотеки
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Добавляем директорию с заголовочными файлами
include_directories(${CMAKE_SOURCE_DIR}/include)

# Список исходных файлов
set(SOURCE_FILES
    src/converter_json.cpp
    src/inverted_index.cpp
    src/search_server.cpp
    src/main.cpp
)

# Создаём исполняемый файл из исходников
add_executable(search_engine ${SOURCE_FILES})

# Линкуем Google Test (для тестового режима)
target_link_libraries(search_engine PRIVATE gtest_main)

# Копирование файлов конфигурации после сборки
add_custom_command(TARGET search_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/config.json"
        $<TARGET_FILE_DIR:search_engine>
)

add_custom_command(TARGET search_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/requests.json"
        $<TARGET_FILE_DIR:search_engine>
)

# Копирование папки resources в итоговую директорию сборки
add_custom_command(TARGET search_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        $<TARGET_FILE_DIR:search_engine>/resources
)

# Включаем систему тестирования
if(BUILD_TESTS)
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(search_engine WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

