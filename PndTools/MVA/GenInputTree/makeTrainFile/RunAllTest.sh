#!/bin/bash

# Script to collect parameters (Features) from given root fles which
# contain simulation data to be used for testing the trained
# classifiers.

NEV=0

############## Fetch Kaon Data #############
Infile=\"/media/daq/babaiexp/TestFiles/RecoKaon07_1.root\"
simFile=\"/media/daq/babaiexp/TestFiles/Kaon07_1.root\"
parFile=\"/media/daq/babaiexp/TestFiles/TestParamMu.root\"
OutPut=\"KaonOutTree.root\"
Tree=\"Kaon\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Muon Data #############
Infile=\"/media/daq/babaiexp/TestFiles/RecoMuon07_1.root\"
simFile=\"/media/daq/babaiexp/TestFiles/Muon07_1.root\"
parFile=\"/media/daq/babaiexp/TestFiles/TestParamMu.root\"
OutPut=\"MuonOutTree.root\"
Tree=\"Muon\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Pion Data #############
Infile=\"/media/daq/babaiexp/TestFiles/RecoPion07_1.root\"
simFile=\"/media/daq/babaiexp/TestFiles/Pion07_1.root\"
parFile=\"/media/daq/babaiexp/TestFiles/TestParamMu.root\"
OutPut=\"PionOutTree.root\"
Tree=\"Pion\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Electron Data #############
Infile=\"/media/daq/babaiexp/TestFiles/RecoElectron07_1.root\"
simFile=\"/media/daq/babaiexp/TestFiles/Electron07_1.root\"
parFile=\"/media/daq/babaiexp/TestFiles/TestParamMu.root\"
OutPut=\"ElectOutTree.root\"
Tree=\"Elect\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Gamma Data #############
Infile=\"/media/daq/babaiexp/TestFiles/RecoGam07_1.root\"
simFile=\"/media/daq/babaiexp/TestFiles/Gam07_1.root\"
parFile=\"/media/daq/babaiexp/TestFiles/TestParamMu.root\"
OutPut=\"GammaOutTree.root\"
Tree=\"Gamma\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Proton Data #############
Infile=\"/media/daq/babaiexp/TestFiles/RecoProt07_1.root\"
simFile=\"/media/daq/babaiexp/TestFiles/Prot07_1.root\"
parFile=\"/media/daq/babaiexp/TestFiles/TestParamMu.root\"
OutPut=\"ProtonOutTree.root\"
Tree=\"Proton\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

## Join the created trees and files into a single file containing all
## the event data
root -l -b -q "extract.C(\"EventFeaturesTest.root\")"
