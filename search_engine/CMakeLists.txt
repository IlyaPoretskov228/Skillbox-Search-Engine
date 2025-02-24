cmake_minimum_required(VERSION 3.15)
project(search_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Политики для подавления предупреждений
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# Опция для сборки тестов (по умолчанию ON)
option(BUILD_TESTS "Build tests" ON)

# Подключаем Google Test через FetchContent
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_disable_pthreads ON)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Подключаем заголовочные файлы
include_directories(${CMAKE_SOURCE_DIR}/include)

# ----------------------------------------------------------------------------
# Создаём статическую библиотеку search_engine_lib, куда входит логика классов
# ----------------------------------------------------------------------------
set(SOURCES_LIB
    src/converter_json.cpp
    src/inverted_index.cpp
    src/search_server.cpp
)

add_library(search_engine_lib STATIC ${SOURCES_LIB})
target_include_directories(search_engine_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# ----------------------------------------------------------------------------
# Основная программа (без тестов)
# ----------------------------------------------------------------------------
add_executable(search_engine src/main.cpp)
target_link_libraries(search_engine PRIVATE search_engine_lib)

# После сборки копируем config.json, requests.json и папку resources
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
add_custom_command(TARGET search_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        $<TARGET_FILE_DIR:search_engine>/resources
)

# ----------------------------------------------------------------------------
# Тесты (подпроект) - отдельный исполняемый файл
# ----------------------------------------------------------------------------
if(BUILD_TESTS)
    enable_testing()

    # Создаём исполняемый файл для тестов
    add_executable(search_engine_tests
        tests/main_tests.cpp     # Единственный main c тестами
    )

    # Линкуем нашу библиотеку + Google Test
    target_link_libraries(search_engine_tests
        PRIVATE
            search_engine_lib
            gtest_main
    )

    include(GoogleTest)
    # Настраиваем автоматический поиск тестов
    gtest_discover_tests(search_engine_tests WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()
