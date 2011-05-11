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
ELosses = ROOT.TH1D("ELosses", "MC Point Energy Losses",
                     500,0,1.E-6)
dEdxs = ROOT.TH1D("dEdxs", "MC Point dE/dx",
                     500,0,1.E-5)
weirdPos = ROOT.TH2D("weirdPos", "pos of weird dEdxs",
                     500,-43,43,500,-43,43)
dE_vs_dx = ROOT.TH2D("dE_vs_dx", "dE vs dx",
                     500,0,1,500,0,1.E-6)

lengths = ROOT.TH1D("lengths", "MC Point stepsizes",
                     500,0,1.)
                     

ROOT.gROOT.ProcessLine(".x rootlogon.C") 
inFile = ROOT.TFile.Open(file, "read")

tree = inFile.Get("cbmsim")
counter = 0

for e in tree :
    for point in tree.PndTpcPoint :
        trackIDs.Fill(point.GetTrackID())
        ELosses.Fill(point.GetEnergyLoss())
        length = point.GetLength()
        lengths.Fill(length)
        dedx = point.GetEnergyLoss()/length
        
        dEdxs.Fill(dedx)
        dE_vs_dx.Fill(length, point.GetEnergyLoss())
        
        if dedx < 1.51E-6 and dedx > 1.49E-6 :
            weirdPos.Fill(point.GetX(), point.GetY())
          
     

c1 = ROOT.TCanvas()
c1.Divide(2,2)
c1.cd(1)
ELosses.Draw()
c1.cd(2)
dEdxs.Draw()
c1.cd(3)
lengths.Draw()

c2 = ROOT.TCanvas()
c2.Divide(2,1)
c2.cd(1)
weirdPos.Draw()
c2.cd(2)
dE_vs_dx.Draw()

inFile.Close()


input()
