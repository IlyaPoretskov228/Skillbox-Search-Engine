cmake_minimum_required(VERSION 3.15)

find_package(Threads REQUIRED)

# Создаём статическую библиотеку search_engine_lib
set(LIB_SOURCES
    converter_json.cpp
    inverted_index.cpp
    search_server.cpp
)

add_library(search_engine_lib STATIC ${LIB_SOURCES})

# Подключаем заголовки в сборку
target_include_directories(search_engine_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/include
)

# Линкуем потоки (обязательно при использовании <thread> и <mutex>)
target_link_libraries(search_engine_lib PRIVATE Threads::Threads)
