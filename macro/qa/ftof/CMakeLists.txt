GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/qa/ftof/QAmacro_ftof_1.C)
add_test(qa_ftof_macro1 ${PANDAROOT_BINARY_DIR}/macro/qa/ftof/QAmacro_ftof_1.sh)
SET_TESTS_PROPERTIES(qa_ftof_macro1 PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(qa_ftof_macro1 PROPERTIES PASS_REGULAR_EXPRESSION "Test Failed;Not Ok")
