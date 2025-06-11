cmake_minimum_required(VERSION 3.15)

# Создаём исполняемый файл для тестов
add_executable(search_engine_tests main_tests.cpp)

# Настройка и загрузка Google Test
if(BUILD_TESTS)
    # Настраиваем загрузку GoogleTest
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    )
    # Для Windows: предотвращаем переопределение настроек
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Добавляем отдельную цель, чтобы избежать проблем с включением
    include(GoogleTest)
    enable_testing()
endif()

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