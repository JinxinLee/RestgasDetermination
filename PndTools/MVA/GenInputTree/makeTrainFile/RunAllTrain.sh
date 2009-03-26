#!/bin/bash

# Script to collect parameters (Features) from given root fles which
# contain simulation data to be used for training the trained
# classifiers.

NEV=0

############## Fetch Kaon Data #############
Infile=\"/media/daq/babaiexp/SimFiles/RecoKaon06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Kaon06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"KaonOutTree.root\"
Tree=\"Kaon\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Muon Data #############
Infile=\"/media/daq/babaiexp/SimFiles/RecoMuon06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Muon06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"MuonOutTree.root\"
Tree=\"Muon\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Pion Data #############
Infile=\"/media/daq/babaiexp/SimFiles/RecoPion06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Pion06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"PionOutTree.root\"
Tree=\"Pion\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Electron Data #############
Infile=\"/media/daq/babaiexp/SimFiles/RecoElectron06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Electron06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"ElectOutTree.root\"
Tree=\"Elect\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Gamma Data #############
Infile=\"/media/daq/babaiexp/SimFiles/RecoGam06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Gam06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"GammaOutTree.root\"
Tree=\"Gamma\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

############## Fetch Proton Data #############
Infile=\"/media/daq/babaiexp/SimFiles/RecoProt06_1.root\"
simFile=\"/media/daq/babaiexp/SimFiles/Prot06_1.root\"
parFile=\"/media/daq/babaiexp/SimFiles/TestParamMu.root\"
OutPut=\"ProtonOutTree.root\"
Tree=\"Proton\"

root -l -b -q "run.C($NEV,$Infile,$simFile,$parFile,$OutPut,$Tree)"

## Join the created trees and files into a single file containing all
## the event data
root -l -b -q "extract.C(\"EventFeaturesTrain.root\")"
