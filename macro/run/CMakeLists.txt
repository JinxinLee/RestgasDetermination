GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/run/sim_complete_stt.C)
add_test(sim_complete_stt ${PANDAROOT_BINARY_DIR}/macro/run/sim_complete_stt.sh)
SET_TESTS_PROPERTIES(sim_complete_stt PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(sim_complete_stt PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/run/digi_complete_stt.C)
add_test(digi_complete_stt ${PANDAROOT_BINARY_DIR}/macro/run/digi_complete_stt.sh)
SET_TESTS_PROPERTIES(digi_complete_stt PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(digi_complete_stt PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/run/reco_complete_stt.C)
add_test(reco_complete_stt ${PANDAROOT_BINARY_DIR}/macro/run/reco_complete_stt.sh)
SET_TESTS_PROPERTIES(reco_complete_stt PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(reco_complete_stt PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/run/sim_complete_tpc.C)
add_test(sim_complete_tpc ${PANDAROOT_BINARY_DIR}/macro/run/sim_complete_tpc.sh)
SET_TESTS_PROPERTIES(sim_complete_tpc PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(sim_complete_tpc PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/run/digi_complete_tpc.C)
add_test(digi_complete_tpc ${PANDAROOT_BINARY_DIR}/macro/run/digi_complete_tpc.sh)
SET_TESTS_PROPERTIES(digi_complete_tpc PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(digi_complete_tpc PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/run/reco_complete_tpc.C)
add_test(reco_complete_tpc ${PANDAROOT_BINARY_DIR}/macro/run/reco_complete_tpc.sh)
SET_TESTS_PROPERTIES(reco_complete_tpc PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(reco_complete_tpc PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")
