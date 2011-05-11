import ROOT, glob, math, sys, os
from ROOT import std

#argument parsing:
for iarg in range(len(sys.argv)) :
    arg = sys.argv[iarg]
    if arg == "--prelim" :
        preliminary = 1
    if arg == "-path" :
        dir = sys.argv[iarg+1];
    if arg == "-n" :
        numFiles = int(sys.argv[iarg+1])
    if arg == "-f":
        file = sys.argv[iarg+1]

trackIDs = ROOT.TH1D("trackIDs", "MC Point track IDs",
                     10,0,10)


ROOT.gROOT.ProcessLine(".x rootlogon.C") 
inFile = ROOT.TFile.Open(file, "read")

tree = inFile.Get("cbmsim")
counter = 0

for e in tree :
    if(counter>1000) :
        break;
    for point in PndTpcPoint :
        trackIDs.Fill(point.GetTrackID())
    counter+=1

input()

inFile.Close()
