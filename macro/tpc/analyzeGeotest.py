import ROOT, glob, math, sys, os
from ROOT import std

batch = 0
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
    if arg == "-b" :
        batch = 1
    
ELosses = ROOT.TH1D("ELosses", "MC Point Energy Losses",
                     500,0,1.E-6)
dEdxs = ROOT.TH1D("dEdxs", "MC Point dE/dx",
                     500,0,1.E-5)
#weirdPos = ROOT.TH2D("weirdPos", "pos of weird dEdxs",
#                     500,-43,43,500,-43,43)
#dE_vs_dx = ROOT.TH2D("dE_vs_dx", "dE vs dx",
#                     500,0,1,500,0,1.E-6)

lengths = ROOT.TH1D("lengths", "MC Point stepsizes",
                     500,0,1.)

totElosses  = ROOT.TH1D("totELosses", "Total energy losses on track",
                        500,0,0.005)
                     

ROOT.gROOT.ProcessLine(".x rootlogon.C") 
inFile = ROOT.TFile.Open(file, "read")

tree = inFile.Get("cbmsim")
counter = 0

for e in tree :
    #define map for Elosses:
    #map <trackID, accumulated ELoss>
    memory = {0:0}
    
    for point in tree.PndTpcPoint :
       
        eLoss = point.GetEnergyLoss()
        ELosses.Fill(eLoss)
        length = point.GetLength()
        lengths.Fill(length)
        dedx = point.GetEnergyLoss()/length
        
        dEdxs.Fill(dedx)
        trackID = point.GetTrackID()
        if trackID in memory :
            memory[trackID] += eLoss
        else :
            memory[trackID] = eLoss
        #dE_vs_dx.Fill(length, point.GetEnergyLoss())
        
    for tid in memory.keys() :
        totElosses.Fill(memory[tid])
        
out = file.replace("mc.root", "histo.root")
outFile = "muh" 
if batch :
    outFile = ROOT.TFile.Open(out, "recreate")
    outFile.cd()

c1 = ROOT.TCanvas()
c1.Divide(2,2)
c1.cd(1)
if batch :
    ELosses.Write()
else:
    ELosses.Draw()
c1.cd(2)
if batch :
    dEdxs.Write()
else:
    dEdxs.Draw()
c1.cd(3)
if batch:
    lengths.Write()
else:
    lengths.Draw()

c2 = ROOT.TCanvas()
if batch:
    totElosses.Write()
else:
    totElosses.Draw()

inFile.Close()
if batch:
    outFile.Close()

if batch == 0 :
    input()
