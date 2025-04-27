cmake_minimum_required(VERSION 3.15)

# Создаём исполняемый файл search_engine (основная программа)
add_executable(search_engine main.cpp)

# Линкуем библиотеку и потоки
target_link_libraries(search_engine
    PRIVATE
    search_engine_lib
    Threads::Threads
)