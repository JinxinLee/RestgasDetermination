GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/qa/lhetrack/run_sim_tpccombi_pgun.C)
add_test(qa_lhe_macro1 ${PANDAROOT_BINARY_DIR}/macro/qa/lhetrack/run_sim_tpccombi.sh)
SET_TESTS_PROPERTIES(qa_lhe_macro1 PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(qa_lhe_macro1 PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/qa/lhetrack/run_digi_tpccombi.C)
add_test(qa_lhe_macro2 ${PANDAROOT_BINARY_DIR}/macro/qa/lhetrack/run_digi_tpccombi.sh)
SET_TESTS_PROPERTIES(qa_lhe_macro2 PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(qa_lhe_macro2 PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/qa/lhetrack/run_reco_tpccombi.C)
add_test(qa_lhe_macro3 ${PANDAROOT_BINARY_DIR}/macro/qa/lhetrack/run_reco_tpccombi.sh)
SET_TESTS_PROPERTIES(qa_lhe_macro3 PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(qa_lhe_macro3 PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")
