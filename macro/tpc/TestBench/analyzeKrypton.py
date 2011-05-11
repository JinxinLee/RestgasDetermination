import ROOT, glob, math, sys, os
from ROOT import std

def drawPrelim() :
    prelim= ROOT.TLatex()
    prelim.SetTextColor(ROOT.kGray)
    prelim.SetTextSize(0.07)
    prelim.SetTextAngle(20)
    prelim.SetNDC()
    prelim.DrawLatex(0.38,0.45,"preliminary")
    return

#default dir
dir = "/nfs/hicran/data/tpc/fopi/2011/reconstructed"
runs = ""

#argument parsing:
for iarg in range(len(sys.argv)) :
    arg = sys.argv[iarg]
    if arg == "--prelim" :
        preliminary = 1
    if arg == "-path" :
        dir = sys.argv[iarg+1];
    if arg == "-runs" :
        runs = sys.argv[iarg+1];    
            

dashIndex = runs.find("-")
if dashIndex > 0 :
    runs = runs.split("-")
    runList = range(int(runs[0]), int(runs[1])+1)
if dashIndex < 0 and len(runs) > 1 :
    runList = [runs]


outfile = ROOT.TFile("KryptonAnaOut.root", "recreate")

ROOT.gROOT.ProcessLine(".x rootlogon.C") 
ROOT.gROOT.ProcessLine('gSystem->Load("libPhysics")')
ROOT.gROOT.ProcessLine('gStyle->SetPalette(1)')

pattern = dir+"/runKr*reco.root"
print pattern

files = glob.glob(pattern)
files.sort()

simpath=""
for env in os.environ.keys() :
    if env == "PANDAPATH" and len(os.environ[env]) > 0 :
        simpath = os.environ[env]
        print "Setting pandaroot path to %s" %simpath

poolfile = simpath + "/tpc/TestBench/PadShapePrototype.dat"
pool = ROOT.PndTpcPadShapePool(poolfile)
planefile = simpath + "/tpc/TestBench/padPlane_FOPI.dat"
plane = ROOT.PndTpcPadPlane(planefile, pool)

clAmp = ROOT.TH1D("clAmp", "Krypton Cluster Amplitude Distribution",
                  500,0,15000)
clAmpCut = ROOT.TH1D("clAmpCut", "Krypton Cluster Amplitude Distribution with cuts",
                     500,0,15000)

dAmp = ROOT.TH1D("dAmp", "Krypton Digi Amplitude Distribution",
                  500,0,2000)


counter=0
for file in files :
    #check if it appears in the runList
    if len(runList) >= 1 :
        found = 0
        for i in runList :
            if file.find(str(i)) >= 0 :
                found = 1
        if found == 0 :
            continue
    
    print(file)
    Rfile = ROOT.TFile.Open(file, "read")
    tree = Rfile.Get("cbmsim")
    tree.SetBranchStatus("*", 0)
    #tree.SetBranchStatus("TrackFitStat.*", 1)
    tree.SetBranchStatus("PndTpcCluster.*", 1)
    tree.SetBranchStatus("PndTpcDigi.*", 1)
   
    print "cbmsim has " + str(tree.GetEntries()) + " entries"
    ecounter = 0
    for e in tree :
        if ecounter % 1000 == 0 :
            print "."
        if ecounter > 15000 :
            break
        
        badCount = 0
        dCount = 0
        for d in e.PndTpcDigi:
            dCount += 1
            damp = d.amp()
#            dAmp.Fill(damp)
            if(damp > 400 and damp < 500) :
                badCount += 1
                
        if dCount > 0 and float(badCount) / float(dCount) > 0.2 :
            print "Bad Event ... skipping"
            continue

        for cl in e.PndTpcCluster :
            clAmp.Fill(cl.amp())
            if(cl.size()<6) :
                continue
            valid = 1
            for d in range(cl.nDigi()) :
                digi = cl.getDigi(d)
                dAmp.Fill(digi.amp())
                if(digi.amp() > 1740) :
                    valid = 0
            if valid == 0 :
                continue
            clAmpCut.Fill(cl.amp())           
        ecounter+=1
                
    Rfile.Close()
    counter+=1        
    
c1 = ROOT.TCanvas()
c1.Divide(2,1)
c1.cd(1)
clAmp.Draw()
c1.cd(2)
clAmpCut.Draw()

c2 = ROOT.TCanvas()
dAmp.Draw()

input()
