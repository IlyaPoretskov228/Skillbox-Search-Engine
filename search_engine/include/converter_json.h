#ifndef CONVERTER_JSON_H
#define CONVERTER_JSON_H

#include <vector>
#include <string>

/**
 * Класс для работы с JSON-файлами (config.json, requests.json, answers.json).
 */
class ConverterJSON {
public:
    ConverterJSON() = default;

    /**
     * Считывает и возвращает содержимое документов, перечисленных в config.json
     */
    std::vector<std::string> GetTextDocuments();

    /**
     * Считывает поле max_responses из config.json
     */
    int GetResponsesLimit();

    /**
     * Считывает и возвращает список запросов из requests.json
     */
    std::vector<std::string> GetRequests();

    /**
     * Записывает результаты поиска в answers.json
     */
    void putAnswers(const std::vector<std::vector<std::pair<int, float>>> &answers);
};

#endif // CONVERTER_JSON_H
