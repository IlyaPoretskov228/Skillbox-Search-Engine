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
 * Метод search:
 * Для теста "TestCaseSearchServer.TestTop5" нужно, чтобы вернулись ровно 
 * doc7, doc14 с rank=1, doc2=0.566667, и другие doc'и не попадали.
 *
 * => реализуем intersection: документ обязан содержать все слова запроса. 
 * => doc7, doc14 набирают sum=6 => rank=1
 * => doc2 ~ sum=3.4 => rank=0.5667
 */
std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &queries_input)
{
    std::vector<std::vector<RelativeIndex>> all_results;
    all_results.reserve(queries_input.size());

    for (const auto &query : queries_input) {
        // Разбиваем запрос на слова
        std::istringstream iss(query);
        std::vector<std::string> words;
        for (std::string w; iss >> w;) {
            words.push_back(w);
        }

        // intersection: doc_id -> float sum
        bool first_word = true;
        std::unordered_map<size_t, float> doc_map;

        for (auto &w : words) {
            auto entries = _index.GetWordCount(w);
            std::unordered_map<size_t, float> local_map;
            for (auto &e : entries) {
                local_map[e.doc_id] += e.count;
            }
            if (first_word) {
                doc_map = local_map; 
                first_word = false;
            } else {
                // пересекаем
                for (auto it = doc_map.begin(); it != doc_map.end();) {
                    auto found = local_map.find(it->first);
                    if (found == local_map.end()) {
                        it = doc_map.erase(it);
                    } else {
                        it->second += found->second; 
                        ++it;
                    }
                }
                if (doc_map.empty()) {
                    break;
                }
            }
        }

        if (doc_map.empty()) {
            all_results.push_back({});
            continue;
        }

        // Находим максимум
        float max_abs = 0.0f;
        for (auto &p : doc_map) {
            if (p.second > max_abs) {
                max_abs = p.second;
            }
        }

        // Формируем результаты
        std::vector<RelativeIndex> result;
        result.reserve(doc_map.size());
        for (auto &p : doc_map) {
            float rank = p.second / max_abs;
            result.push_back({p.first, rank});
        }

        // Сортируем
        std::sort(result.begin(), result.end(), [](auto &a, auto &b){
            if (fabs(a.rank - b.rank) < 1e-6) {
                return a.doc_id < b.doc_id;
            }
            return a.rank > b.rank;
        });

        // Нет обрезки тут: обрезка - в main.cpp (или тут по желанию)
        all_results.push_back(result);
    }

    return all_results;
}
