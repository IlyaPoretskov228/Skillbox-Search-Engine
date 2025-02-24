#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include <vector>
#include <string>
#include <map>
#include <cstddef>

/**
 * Структура для хранения doc_id и частоты слова (count).
 */
struct Entry {
    size_t doc_id;
    size_t count;
    bool operator==(const Entry &other) const;
};

/**
 * Класс для многопоточной индексации текстовых документов.
 */
class InvertedIndex {
public:
    InvertedIndex() = default;

    /**
     * Обновляет или заполняет базу документов.
     */
    void UpdateDocumentBase(const std::vector<std::string> &input_docs);

    /**
     * Возвращает список Entry для заданного слова.
     */
    std::vector<Entry> GetWordCount(const std::string &word) const;

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;
};

#endif // INVERTED_INDEX_H
