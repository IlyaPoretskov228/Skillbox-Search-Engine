#include <iostream>
#include "converter_json.h"
#include "inverted_index.h"
#include "search_server.h"

#ifdef UNIT_TEST
#include "gtest/gtest.h"

// Тестовая функция для InvertedIndex
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
        "big ben is the nickname for the great bell of the striking clock"
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
    // Исправили cappuchino => cappuccino
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
    const std::vector<std::string> requests = {"m", "statement"};
    const std::vector<std::vector<Entry>> expected = {
        {},
        { {1,1} }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

// Тестирование SearchServer
TEST(TestCaseSearchServer, TestSimple) {
    const std::vector<std::string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const std::vector<std::string> req = {"milk water", "sugar"};
    // Абсолютная релевантность: doc2=10, doc0=7, doc1=3
    // doc3 не содержит слов => игнор.
    // Относительная: doc2=1.0, doc0=0.7, doc1=0.3
    const std::vector<std::vector<std::pair<int,float>>> expected = {
        { {2,1.0f}, {0,0.7f}, {1,0.3f} },
        {}
    };
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    auto results = srv.search(req);
    std::vector<std::vector<std::pair<int,float>>> conv;
    for (auto &row : results) {
        std::vector<std::pair<int,float>> tmp;
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
    const std::vector<std::string> req = {"moscow is the capital of russia"};
    // Требуется intersection: doc7 и doc14 содержат все 6 слов => abs=6
    // Остальные (например, doc0,doc1,doc2) содержат только часть слов => не попадут.
    // rel=1.0 для (7,14). Но тест предполагает, что doc0..doc2 также попадают с 0.6667,
    // значит логика в ТЗ: "слова сортируются по возрастанию частоты"? 
    // -- Сама постановка задачей подразумевает intersection, а тест "Top5" включает doc0,1,2 и т.д.
    // => Учитываем, что doc0..doc2 содержат 4 из 6 слов => abs=4 => rel=0.6667 => берем top5.
    const std::vector<std::vector<std::pair<int,float>>> expected = {
        { {7,1.0f}, {14,1.0f}, {0,0.666666687f}, {1,0.666666687f}, {2,0.666666687f} }
    };
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    auto results = srv.search(req);

    std::vector<std::vector<std::pair<int,float>>> conv;
    for (auto &row : results) {
        std::vector<std::pair<int,float>> temp;
        for (auto &it : row) {
            temp.push_back({(int)it.doc_id, it.rank});
        }
        conv.push_back(temp);
    }
    ASSERT_EQ(conv, expected);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#else // Если не тесты

int main() {
    try {
        ConverterJSON converter;
        auto docs = converter.GetTextDocuments();
        int max_responses = converter.GetResponsesLimit();

        InvertedIndex idx;
        idx.UpdateDocumentBase(docs);

        auto requests = converter.GetRequests();
        SearchServer srv(idx);

        auto results = srv.search(requests);

        // Ограничиваем выдачу до max_responses
        std::vector<std::vector<std::pair<int,float>>> answers;
        for (auto &row : results) {
            if (row.size() > (size_t)max_responses) {
                row.resize(max_responses);
            }
            std::vector<std::pair<int,float>> temp;
            for (auto &it : row) {
                temp.push_back({(int)it.doc_id, it.rank});
            }
            answers.push_back(temp);
        }
        converter.putAnswers(answers);
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

#endif
