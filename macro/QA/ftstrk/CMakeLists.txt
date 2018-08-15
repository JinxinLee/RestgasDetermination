GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/QA/ftstrk/sim.C)
add_test(sim_ftstrk ${PANDAROOT_BINARY_DIR}/macro/QA/ftstrk/sim.sh)
SET_TESTS_PROPERTIES(sim_ftstrk PROPERTIES TIMEOUT "400")
SET_TESTS_PROPERTIES(sim_ftstrk PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully")

GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/QA/ftstrk/digi.C)
add_test(digi_ftstrk ${PANDAROOT_BINARY_DIR}/macro/QA/ftstrk/digi.sh)
SET_TESTS_PROPERTIES(digi_ftstrk PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES(digi_ftstrk PROPERTIES DEPENDS sim_ftstrk)
SET_TESTS_PROPERTIES(digi_ftstrk PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully")

#GENERATE_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/QA/ftstrk/reco.C)
#add_test(reco_ftstrk ${PANDAROOT_BINARY_DIR}/macro/QA/ftstrk/reco.sh)
#SET_TESTS_PROPERTIES(reco_ftstrk PROPERTIES TIMEOUT "200")
#SET_TESTS_PROPERTIES(reco_ftstrk PROPERTIES DEPENDS digi_ftstrk)
#SET_TESTS_PROPERTIES(reco_ftstrk PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully")

#Generate_ROOT_TEST_SCRIPT(${PANDAROOT_SOURCE_DIR}/macro/QA/ftstrk/QA_histos.C)
#add_test(qahistos_ftstrk ${PANDAROOT_BINARY_DIR}/macro/QA/ftstrk/QA_histos.sh)
#SET_TESTS_PROPERTIES(qahistos_ftstrk PROPERTIES TIMEOUT "200")
#SET_TESTS_PROPERTIES(qahistos_ftstrk PROPERTIES DEPENDS recoqa_ftstrk)
#SET_TESTS_PROPERTIES(qahistos_ftstrk PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully")

