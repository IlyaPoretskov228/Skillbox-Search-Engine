add_test([=[TestCaseInvertedIndex.TestBasic]=]  C:/search_engine/build/Debug/search_engine.exe [==[--gtest_filter=TestCaseInvertedIndex.TestBasic]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestCaseInvertedIndex.TestBasic]=]  PROPERTIES WORKING_DIRECTORY C:/search_engine/build/Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[TestCaseInvertedIndex.TestBasic2]=]  C:/search_engine/build/Debug/search_engine.exe [==[--gtest_filter=TestCaseInvertedIndex.TestBasic2]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestCaseInvertedIndex.TestBasic2]=]  PROPERTIES WORKING_DIRECTORY C:/search_engine/build/Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[TestCaseInvertedIndex.TestInvertedIndexMissingWord]=]  C:/search_engine/build/Debug/search_engine.exe [==[--gtest_filter=TestCaseInvertedIndex.TestInvertedIndexMissingWord]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestCaseInvertedIndex.TestInvertedIndexMissingWord]=]  PROPERTIES WORKING_DIRECTORY C:/search_engine/build/Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[TestCaseSearchServer.TestSimple]=]  C:/search_engine/build/Debug/search_engine.exe [==[--gtest_filter=TestCaseSearchServer.TestSimple]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestCaseSearchServer.TestSimple]=]  PROPERTIES WORKING_DIRECTORY C:/search_engine/build/Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[TestCaseSearchServer.TestTop5]=]  C:/search_engine/build/Debug/search_engine.exe [==[--gtest_filter=TestCaseSearchServer.TestTop5]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestCaseSearchServer.TestTop5]=]  PROPERTIES WORKING_DIRECTORY C:/search_engine/build/Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  search_engine_TESTS TestCaseInvertedIndex.TestBasic TestCaseInvertedIndex.TestBasic2 TestCaseInvertedIndex.TestInvertedIndexMissingWord TestCaseSearchServer.TestSimple TestCaseSearchServer.TestTop5)
