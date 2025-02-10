#ifndef SEARCH_SERVER_H
#define SEARCH_SERVER_H

#include <vector>
#include <string>
#include <cstddef>
#include "inverted_index.h"

/**
 * Структура для хранения doc_id и относительной релевантности (rank).
 */
struct RelativeIndex {
    size_t doc_id;
    float rank;
    bool operator==(const RelativeIndex &other) const;
};

/**
 * Класс для обработки поисковых запросов и вычисления релевантности.
 */
class SearchServer {
public:
    /**
     * Конструктор принимает ссылку на InvertedIndex
     */
    SearchServer(InvertedIndex &idx);

    /**
     * Выполняет поиск по списку запросов. Возвращает вектор результатов,
     * где каждый элемент - вектор RelativeIndex (документ, rank).
     */
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string> &queries_input);

private:
    InvertedIndex &_index;
};

#endif // SEARCH_SERVER_H
