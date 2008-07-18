# Copyright (C) 2005 Florian Uhlig <f.uhlig@gsi.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST 		= reconf configure m4 macro
CLEANFILES		= *~

if COMP_GEANT4
  GEANT4_DIR=cbmg4
endif

if COMP_GEANT3
  GEANT3_DIR=bla
endif

if COMP_ORACLE
  ORACLE_DIR=ora
endif

if COND_HADES
  HADES_DIR=hades
endif


SUBDIRS			= geobase parbase base trackbase mcstack field generators pgenerators passive stt tpc tpc/tpcreco mvd muo emc drc drc/drcprop hyp genfit recotasks dch tof rpc lhetrack $(GEANT4_DIR) 


dist-hook:
	find . -name '*Dict.cxx' | xargs rm -f 
	find . -name '*Dict.h' | xargs rm -f	

cleanall:
	make distclean
	rm -rf lib include
#
# EOF
#
