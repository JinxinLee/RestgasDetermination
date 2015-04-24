# -*- mode: cmake -*-

#message(" -- Read CTestCustom.cmake --")

# -----------------------------------------------------------
# -- Number of warnings to display
# -----------------------------------------------------------

set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS "500" )

# -----------------------------------------------------------
# -- Number of errors to display
# -----------------------------------------------------------

set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS   "50" )

set(CTEST_CUSTOM_ERROR_EXCEPTION
       ${CTEST_CUSTOM_ERROR_EXCEPTION}
       "Warnung: "
       "Anmerkung: "
)

# -----------------------------------------------------------
# -- Warning execptions
# -----------------------------------------------------------

set(CTEST_CUSTOM_WARNING_EXCEPTION
	${CTEST_CUSTOM_WARNING_EXCEPTION}

        # -- warnings from ubuntu systems which are a little to much
        # -- probably defined warn-unused-result. ignoring the result
        # -- of fgets is common practice. A work around would be to
        # -- store the return value in a dummy variable
        "ignoring return value of 'char* fgets(char*, int, FILE*)'"
        "ignoring return value of 'char* fscanf(char*, int, FILE*)'"

       # -- EvtGen source code, cannot be touched by us 
        "pgenerators/EvtGen/EvtGen/R01-03-00/"
        "pgenerators/EvtGen/Photos/"
        "genfit/"
        "genfit2/"
        "/include/HepMC/"
  )

# -----------------------------------------------------------
# -- Warning addon's
# -----------------------------------------------------------
set(CTEST_CUSTOM_WARNING_MATCH	${CTEST_CUSTOM_WARNING_MATCH}
	)

Set (CTEST_CUSTOM_COVERAGE_EXCLUDE 
     "Dict.h"
     "Dict.cxx"
    )

# -----------------------------------------------------------
# -- Error execptions
# -- Get rid of boost warnings which are misinterpreted as errors
# -----------------------------------------------------------

Set(CTEST_CUSTOM_ERROR_EXCEPTION
	${CTEST_CUSTOM_ERROR_EXCEPTION}
        "/include/boost/"
)

