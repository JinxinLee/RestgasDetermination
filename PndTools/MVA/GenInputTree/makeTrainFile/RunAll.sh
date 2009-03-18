#!/bin/bash

NEV=0

Infile=\"/media/daq/babaiexp/SimFiles/RecoKaon06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Kaon06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"KaonOutTree.root\"
Tree=\"Kaon\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"


Infile=\"/media/daq/babaiexp/SimFiles/RecoMuon06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Muon06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"MuonOutTree.root\"
Tree=\"Muon\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"


Infile=\"/media/daq/babaiexp/SimFiles/RecoPion06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Pion06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"PionOutTree.root\"
Tree=\"Pion\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"


root -l -b -q "extract.C"
