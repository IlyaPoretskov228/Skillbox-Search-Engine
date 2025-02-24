#include "search_server.h"
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>

bool RelativeIndex::operator==(const RelativeIndex &other) const {
    return doc_id == other.doc_id && std::fabs(rank - other.rank) < 1e-6;
}

SearchServer::SearchServer(InvertedIndex &idx)
    : _index(idx)
{}

/**
 * Метод поиска (частичное совпадение):
 *  - Суммируем count для всех слов запроса во всех документах.
 *  - Вычисляем max_abs.
 *  - Относительная релевантность = abs / max_abs.
 *  - Сортируем по убыванию rank, при равенстве doc_id.
 *  - Не обрезаем здесь: можно обрезать в main.cpp или здесь по требованию.
 */
std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &queries_input)
{
    std::vector<std::vector<RelativeIndex>> all_results;
    all_results.reserve(queries_input.size());

    for (auto &query : queries_input) {
        std::unordered_map<size_t, size_t> doc_relevance;
        // Разбиваем запрос на слова
        std::istringstream iss(query);
        std::string word;
        while (iss >> word) {
            // Находим, в каких документах встречается слово
            auto entries = _index.GetWordCount(word);
            for (auto &e : entries) {
                doc_relevance[e.doc_id] += e.count; // size_t -> size_t (нет предупреждения C4267)
            }
        }
        if (doc_relevance.empty()) {
            // Если документов нет
            all_results.push_back({});
            continue;
        }
        // Находим maximum
        size_t max_abs = 0;
        for (auto &kv : doc_relevance) {
            if (kv.second > max_abs) {
                max_abs = kv.second;
            }
        }
        // Вычисляем ранги
        std::vector<RelativeIndex> result;
        result.reserve(doc_relevance.size());
        for (auto &kv : doc_relevance) {
            float rank = static_cast<float>(kv.second) / static_cast<float>(max_abs);
            result.push_back({kv.first, rank});
        }
        // Сортируем по убыванию rank, при равенстве - по doc_id
        std::sort(result.begin(), result.end(), [](const RelativeIndex &a, const RelativeIndex &b){
            if (std::fabs(a.rank - b.rank) < 1e-6) {
                return a.doc_id < b.doc_id;
            }
            return a.rank > b.rank;
        });

        // (Обрезание Top5 можно делать тут или в main.cpp)

        all_results.push_back(result);
    }
    return all_results;
}
