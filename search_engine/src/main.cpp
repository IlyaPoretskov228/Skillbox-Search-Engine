#include <iostream>
#include "converter_json.h"
#include "inverted_index.h"
#include "search_server.h"

int main() {
    try {
        ConverterJSON converter;
        // Считываем документы из config.json
        auto docs = converter.GetTextDocuments();
        // Считываем лимит
        int max_responses = converter.GetResponsesLimit();

        // Индексируем документы
        InvertedIndex idx;
        idx.UpdateDocumentBase(docs);

        // Считываем запросы
        auto requests = converter.GetRequests();

        // Поиск
        SearchServer srv(idx);
        auto results = srv.search(requests);

        // Обрезаем до max_responses
        std::vector<std::vector<std::pair<int, float>>> answers;
        answers.reserve(results.size());
        for (auto &row : results) {
            // Оставляем только TopN
            if ((int)row.size() > max_responses) {
                row.resize(max_responses);
            }
            std::vector<std::pair<int, float>> temp;
            temp.reserve(row.size());
            for (auto &item : row) {
                temp.push_back({(int)item.doc_id, item.rank});
            }
            answers.push_back(temp);
        }

        // Выводим в answers.json
        converter.putAnswers(answers);
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
