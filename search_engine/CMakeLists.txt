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

# Подпроект библиотеки (lib/)
add_subdirectory(lib)

# Создаём исполняемый файл search_engine (основная программа)
add_executable(search_engine src/main.cpp)
# Линкуем нашу библиотеку (search_engine_lib)
target_link_libraries(search_engine PRIVATE search_engine_lib)

# Копирование config.json, requests.json и resources после сборки
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

# Если хотим собирать тесты - подключаем подпроект tests/
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
