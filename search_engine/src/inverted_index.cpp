#include "inverted_index.h"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cctype>

static std::string to_lower(const std::string &s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return res;
}

bool Entry::operator==(const Entry &other) const {
    return doc_id == other.doc_id && count == other.count;
}

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string> &input_docs) {
    docs = input_docs;
    freq_dictionary.clear();

    std::vector<std::thread> threads;
    std::mutex mtx;

    for (size_t i = 0; i < docs.size(); i++) {
        threads.emplace_back([this, i, &mtx]() {
            std::istringstream iss(docs[i]);
            std::string word;
            std::unordered_map<std::string, size_t> local_count;
            while (iss >> word) {
                word = to_lower(word);
                local_count[word]++;
            }
            {
                std::lock_guard<std::mutex> lock(mtx);
                for (auto &p : local_count) {
                    freq_dictionary[p.first].push_back({i, p.second});
                }
            }
        });
    }

    for (auto &t : threads) {
        t.join();
    }

    // Сортируем каждую группу Entry по doc_id
    for (auto &kv : freq_dictionary) {
        auto &entries = kv.second;
        std::sort(entries.begin(), entries.end(),
                  [](const Entry &a, const Entry &b){
                      return a.doc_id < b.doc_id;
                  });
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string &word) const {
    auto lw = to_lower(word);
    auto it = freq_dictionary.find(lw);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}
