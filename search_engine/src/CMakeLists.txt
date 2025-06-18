cmake_minimum_required(VERSION 3.15)

# Создаём исполняемый файл search_engine (основная программа)
add_executable(search_engine main.cpp)

# Линкуем нашу библиотеку (search_engine_lib) + потоки
target_link_libraries(search_engine
        PRIVATE
        search_engine_lib
        Threads::Threads
)

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