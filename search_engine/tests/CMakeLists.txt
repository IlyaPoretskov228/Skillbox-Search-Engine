cmake_minimum_required(VERSION 3.15)

# Создаём исполняемый файл для тестов
add_executable(search_engine_tests main_tests.cpp)

# Линкуем search_engine_lib + GoogleTest
target_link_libraries(search_engine_tests
    PRIVATE
        search_engine_lib
        gtest_main
)

# Автоматическое обнаружение тестов
include(GoogleTest)
gtest_discover_tests(search_engine_tests WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
