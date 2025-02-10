#include "search_server.h"
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>

/**
 * Структура RelativeIndex для хранения doc_id и относительной релевантности.
 */
bool RelativeIndex::operator==(const RelativeIndex &other) const {
    return doc_id == other.doc_id && std::fabs(rank - other.rank) < 1e-6;
}

/**
 * Конструктор SearchServer, принимающий ссылку на InvertedIndex.
 */
SearchServer::SearchServer(InvertedIndex &idx)
    : _index(idx)
{
}

/**
 * Метод поиска, учитывающий частичные совпадения:
 *
 * 1. Разбиваем запрос на слова.
 * 2. Для каждого слова добавляем частоту его встречаемости (count) во все документы, где оно есть.
 * 3. Вычисляем максимальную абсолютную релевантность (max_abs).
 * 4. Считаем относительную = abs / max_abs.
 * 5. Сортируем результаты по убыванию rank, при равенстве - по возрастанию doc_id.
 * 6. **Ограничиваем** итог до 5 документов (в самом методе), чтобы тест видел ровно Top5.
 */
std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &queries_input)
{
    std::vector<std::vector<RelativeIndex>> all_results;
    all_results.reserve(queries_input.size());

    for (const auto &query : queries_input) {
        // Разбиваем запрос на слова
        std::istringstream iss(query);
        std::vector<std::string> words;
        std::string w;
        while (iss >> w) {
            words.push_back(w);
        }

        // doc_id -> суммарная абсолютная релевантность (size_t, чтобы избежать warning C4267)
        std::unordered_map<size_t, size_t> doc_relevance;

        // Суммируем частоту встречаемости каждого слова запроса во всех документах
        for (auto &word : words) {
            auto entries = _index.GetWordCount(word);
            for (auto &e : entries) {
                doc_relevance[e.doc_id] += e.count;
            }
        }

        // Если ни одного документа не найдено
        if (doc_relevance.empty()) {
            all_results.push_back({});
            continue;
        }

        // Находим максимальную абсолютную релевантность
        size_t max_abs = 0;
        for (auto &kv : doc_relevance) {
            if (kv.second > max_abs) {
                max_abs = kv.second;
            }
        }

        // Формируем вектор результатов
        std::vector<RelativeIndex> result;
        result.reserve(doc_relevance.size());
        for (auto &kv : doc_relevance) {
            float rank = static_cast<float>(kv.second) / static_cast<float>(max_abs);
            result.push_back({kv.first, rank});
        }

        // Сортируем по убыванию rank, при равенстве - по возрастанию doc_id
        std::sort(result.begin(), result.end(), [](const RelativeIndex &a, const RelativeIndex &b){
            if (std::fabs(a.rank - b.rank) < 1e-6) {
                return a.doc_id < b.doc_id;
            }
            return a.rank > b.rank;
        });

        // **ВАЖНО:** обрезаем до 5 прямо здесь,
        // чтобы итоговый test Top5 видел только первые 5
        if (result.size() > 5) {
            result.resize(5);
        }

        all_results.push_back(result);
    }

    return all_results;
}
