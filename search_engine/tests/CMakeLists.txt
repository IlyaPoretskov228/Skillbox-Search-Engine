cmake_minimum_required(VERSION 3.15)

# Создаём исполняемый файл для тестов
add_executable(search_engine_tests main_tests.cpp)

# Включаем поддержку тестов в проект
enable_testing()

# Линкуем search_engine_lib + GoogleTest
target_link_libraries(search_engine_tests
    PRIVATE
        search_engine_lib
        gtest
        gtest_main
        Threads::Threads
)

# Добавляем директории включения для тестов
target_include_directories(search_engine_tests 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/lib
        ${gtest_SOURCE_DIR}/include
)

# Автоматическое обнаружение тестов
gtest_discover_tests(search_engine_tests WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")