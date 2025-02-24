#include <gtest/gtest.h>
#include "converter_json.h"
#include "inverted_index.h"
#include "search_server.h"

/**
 * Тесты InvertedIndex
 */

void TestInvertedIndexFunctionality(const std::vector<std::string> &docs,
                                    const std::vector<std::string> &reqs,
                                    const std::vector<std::vector<Entry>> &expected)
{
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    std::vector<std::vector<Entry>> result;
    for (auto &r : reqs) {
        result.push_back(idx.GetWordCount(r));
    }
    ASSERT_EQ(result, expected);
}

TEST(TestCaseInvertedIndex, TestBasic) {
    const std::vector<std::string> docs = {
        "london is the capital of great britain",
        "big ben is the nickname for the Great bell of the striking clock"
    };
    const std::vector<std::string> requests = {"london", "the"};
    const std::vector<std::vector<Entry>> expected = {
        { {0, 1} },
        { {0, 1}, {1, 3} }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestBasic2) {
    const std::vector<std::string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    // заменили cappuchino -> cappuccino
    const std::vector<std::string> requests = {"milk", "water", "cappuccino"};
    const std::vector<std::vector<Entry>> expected = {
        { {0,4},{1,1},{2,5} },
        { {0,3},{1,2},{2,5} },
        { {3,1} }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestInvertedIndexMissingWord) {
    const std::vector<std::string> docs = {
        "a b c d e f g h i j k l",
        "statement"
    };
    const std::vector<std::string> reqs = {"m", "statement"};
    const std::vector<std::vector<Entry>> expected = {
        {},
        { {1,1} }
    };
    TestInvertedIndexFunctionality(docs, reqs, expected);
}

/**
 * Тесты SearchServer
 */

TEST(TestCaseSearchServer, TestSimple) {
    const std::vector<std::string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const std::vector<std::string> reqs = {"milk water", "sugar"};
    const std::vector<std::vector<std::pair<int,float>>> expected = {
        { {2,1.0f}, {0,0.7f}, {1,0.3f} },
        {}
    };
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    auto results = srv.search(reqs);

    // Преобразуем RelativeIndex => пары (doc_id, rank)
    std::vector<std::vector<std::pair<int,float>>> conv;
    conv.reserve(results.size());
    for (auto &row : results) {
        std::vector<std::pair<int,float>> tmp;
        tmp.reserve(row.size());
        for (auto &it : row) {
            tmp.push_back({(int)it.doc_id, it.rank});
        }
        conv.push_back(tmp);
    }
    ASSERT_EQ(conv, expected);
}

TEST(TestCaseSearchServer, TestTop5) {
    const std::vector<std::string> docs = {
        "london is the capital of great britain",
        "paris is the capital of france",
        "berlin is the capital of germany",
        "rome is the capital of italy",
        "madrid is the capital of spain",
        "lisboa is the capital of portugal",
        "bern is the capital of switzerland",
        "moscow is the capital of russia",
        "kiev is the capital of ukraine",
        "minsk is the capital of belarus",
        "astana is the capital of kazakhstan",
        "beijing is the capital of china",
        "tokyo is the capital of japan",
        "bangkok is the capital of thailand",
        "welcome to moscow the capital of russia the third rome",
        "amsterdam is the capital of netherlands",
        "helsinki is the capital of finland",
        "oslo is the capital of norway",
        "stockholm is the capital of sweden",
        "riga is the capital of latvia",
        "tallinn is the capital of estonia",
        "warsaw is the capital of poland"
    };
    const std::vector<std::string> reqs = {"moscow is the capital of russia"};
    const std::vector<std::vector<std::pair<int,float>>> expected = {
        { {7,1.0f}, {14,1.0f}, {0,0.6666667f}, {1,0.6666667f}, {2,0.6666667f} }
    };

    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    auto results = srv.search(reqs);

    std::vector<std::vector<std::pair<int,float>>> conv;
    conv.reserve(results.size());
    for (auto &row : results) {
        std::vector<std::pair<int,float>> tmp;
        tmp.reserve(row.size());
        for (auto &it : row) {
            tmp.push_back({(int)it.doc_id, it.rank});
        }
        conv.push_back(tmp);
    }
    ASSERT_EQ(conv, expected);
}

// Точка входа для тестов
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
