add_test(run_sim ${ROOTSYS}/bin/root -l -b ${PANDAROOT_SOURCE_DIR}/macro/run/run_sim.C)
set_tests_properties ( run_sim PROPERTIES TIMEOUT 300)
SET_TESTS_PROPERTIES ( run_sim PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

add_test(run_sim1 ${ROOTSYS}/bin/root -l -b ${PANDAROOT_SOURCE_DIR}/macro/run/run_sim1.C)
set_tests_properties ( run_sim1 PROPERTIES TIMEOUT 300)
SET_TESTS_PROPERTIES ( run_sim1 PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")
