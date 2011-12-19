import ROOT, glob, math, sys, os
from ROOT import std


vertexfile=""
maxEvents = 0
exclude = False 

#argument parsing:
for iarg in range(len(sys.argv)) :
    arg = sys.argv[iarg]
    if arg == "-f" :
        vertexfile = sys.argv[iarg+1]   
    if arg == "-nEvents" :
        maxEvents = int( sys.argv[iarg+1] )
    if arg == "-excl" :
        exclude = True
#finished argument parsing


ROOT.gROOT.ProcessLine(".x rootlogon.C") 
#ROOT.gROOT.ProcessLine(".x rootlogon_BernhardSmallFont.C") 
#ROOT.gROOT.ProcessLine('gROOT->SetStyle("col");')
#ROOT.gROOT.ProcessLine('gROOT->ForceStyle();') 
ROOT.gROOT.ProcessLine('gROOT->SetStyle("Plain");') 
ROOT.gROOT.ProcessLine('gStyle->SetPalette(1);') 
ROOT.gROOT.ProcessLine('gStyle->SetOptFit(0011);') 
#ROOT.gROOT.ProcessLine(".L macro/tpc/TestBench/pythonContainers.C+")


#  ---------------------------------------- ANA LOOP --------------------------------------

outname = vertexfile[0:vertexfile.find(".root")]
if exclude :
    outname += ".exclude"
outname += ".analysis.root"
print outname
outfile = ROOT.TFile(outname, "recreate")


rMax = 10.
zMin = -15.
zMax = 50.

nBins = 250

nTr = ROOT.TH1D("nTr", "number of Tracks", 20, 0, 20)
nTr.GetXaxis().SetTitle("number of tracks")

CdcX = ROOT.TH1D("CdcX", "X distribution of CDC vertices", nBins, -rMax, rMax) 
CdcX.GetXaxis().SetTitle("x (cm)")
CdcY = ROOT.TH1D("CdcY", "Y distribution of CDC vertices", nBins, -rMax, rMax) 
CdcY.GetXaxis().SetTitle("y (cm)")
CdcZ = ROOT.TH1D("CdcZ", "Z distribution of CDC vertices", nBins, zMin, zMax) 
CdcZ.GetXaxis().SetTitle("z (cm)")

CdcXY = ROOT.TH2D("CdcXY", "XY distribution of CDC vertices", nBins, -rMax, rMax, nBins, -rMax, rMax) 
CdcXY.GetXaxis().SetTitle("x (cm)")
CdcXY.GetYaxis().SetTitle("y (cm)")
CdcXZ = ROOT.TH2D("CdcRZ", "XZ distribution of CDC vertices", nBins, zMin, zMax, nBins, -rMax, rMax) 
CdcXZ.GetXaxis().SetTitle("z (cm)")
CdcXZ.GetYaxis().SetTitle("x (cm)")
CdcYZ = ROOT.TH2D("CdcRZ", "YZ distribution of CDC vertices", nBins, zMin, zMax, nBins, -rMax, rMax)
CdcYZ.GetXaxis().SetTitle("z (cm)")
CdcYZ.GetYaxis().SetTitle("y (cm)")

CDCnVert = ROOT.TH1D("CDCnVert", "number of CDC Vertices", 5, 0, 5) 
CDCnVert.GetXaxis().SetTitle("number of vertices")
nCdcVertVsnTr = ROOT.TH2D("nVertVsnTr", "#CDC Vertices vs. #Tracks", 20, 0, 20, 5, 0, 5)
nCdcVertVsnTr.GetXaxis().SetTitle("number of tracks")
nCdcVertVsnTr.GetYaxis().SetTitle("number of vertices")

CombX = ROOT.TH1D("CombX", "X distribution of CDC+TPC vertices", nBins, -rMax, rMax) 
CombX.GetXaxis().SetTitle("x (cm)")
CombY = ROOT.TH1D("CombY", "Y distribution of CDC+TPC vertices", nBins, -rMax, rMax) 
CombY.GetXaxis().SetTitle("y (cm)")
CombZ = ROOT.TH1D("CombZ", "Z distribution of CDC+TPC vertices", nBins, zMin, zMax) 
CombZ.GetXaxis().SetTitle("z (cm)")

CombXY = ROOT.TH2D("CombXY", "XY distribution of CDC+TPC vertices", nBins, -rMax, rMax, nBins, -rMax, rMax) 
CombXY.GetXaxis().SetTitle("x (cm)")
CombXY.GetYaxis().SetTitle("y (cm)")
CombXZ = ROOT.TH2D("CombRZ", "XZ distribution of CDC+TPC vertices", nBins, zMin, zMax, nBins, -rMax, rMax) 
CombXZ.GetXaxis().SetTitle("z (cm)")
CombXZ.GetYaxis().SetTitle("x (cm)")
CombYZ = ROOT.TH2D("CombRZ", "YZ distribution of CDC+TPC vertices", nBins, zMin, zMax, nBins, -rMax, rMax)
CombYZ.GetXaxis().SetTitle("z (cm)")
CombYZ.GetYaxis().SetTitle("y (cm)")

CombnVert = ROOT.TH1D("CombnVert", "number of CDC+TPC Vertices", 5, 0, 5) 
CombnVert.GetXaxis().SetTitle("number of vertices")
nCombVertVsnTr = ROOT.TH2D("nVertVsnTr", "#CDC+TPC Vertices vs. #Tracks", 20, 0, 20, 5, 0, 5)
nCombVertVsnTr.GetXaxis().SetTitle("number of tracks")
nCombVertVsnTr.GetYaxis().SetTitle("number of vertices")


DX = ROOT.TH1D("DX", "dx", 200, -10, 10) 
DX.GetXaxis().SetTitle("dx (cm)")
DY = ROOT.TH1D("DY", "dy", 200, -10, 10) 
DY.GetXaxis().SetTitle("dy (cm)")
DZ = ROOT.TH1D("DZ", "dz", 200, -30, 30) 
DZ.GetXaxis().SetTitle("dz (cm)")



recofile = vertexfile[0:vertexfile.find(".vertexing.")]
recofile += ".reco.root"

print "Processing files: "
print(vertexfile)
print(recofile)




VFile = ROOT.TFile.Open(vertexfile, "read")
tree = VFile.Get("cbmsim")
if tree == None:
    print "did not find cbmsim"
    VFile.Close()
nEvents = tree.GetEntriesFast()
tree.AddFriend("cbmsim", recofile)

if maxEvents!=0 and maxEvents<nEvents :
    nEvents=maxEvents
        
  
tree.SetBranchStatus("*", 0)
tree.SetBranchStatus("GFVertex.*", 1)
#tree.SetBranchStatus("EventHeader.*", 1)
#tree.SetBranchStatus("GFVertex.fComponents", 0)
#tree.SetBranchStatus("GFVertex.fPos", 1)
tree.SetBranchStatus("PndTpcCdcEvent.*", 1)
tree.SetBranchStatus("PndTpcCdcPreFit.*", 1)


count = -1





for e in tree :        
    count += 1
    if count%250==0 :
        print "At event %i" %count  
    
    if count+2 == nEvents  :
        break
       
    nTracks = e.PndTpcCdcPreFit.GetEntriesFast()
    nTr.Fill(nTracks)
    
    # distributions of CDC+TPC vertices
    for vtx in e.GFVertex :
        pos = vtx.getPos()
        CombX.Fill(pos.X())
        CombY.Fill(pos.Y())
        CombZ.Fill(pos.Z())
        
        CombXY.Fill(pos.X(), pos.Y())
        CombXZ.Fill(pos.Z(), pos.X())
        CombYZ.Fill(pos.Z(), pos.Y())
        
    CombnVert.Fill(e.GFVertex.GetEntriesFast())    
    nCombVertVsnTr.Fill(nTracks, e.GFVertex.GetEntriesFast())
    
    # distributions of CDC vertices
    nCdcVert = 0
    for evt in e.PndTpcCdcEvent :
        vtx = evt.GetVertex()
        if vtx.X()!=0 and vtx.Y()!=0 and vtx.Z()!=0 and not(exclude and nTracks<2) :
        
            nCdcVert += 1
                    
            CdcX.Fill(vtx.X())
            CdcY.Fill(vtx.Y())
            CdcZ.Fill(vtx.Z())
                   
            CdcXY.Fill(vtx.X(), vtx.Y())
            CdcXZ.Fill(vtx.Z(), vtx.X())
            CdcYZ.Fill(vtx.Z(), vtx.Y())
        
    CDCnVert.Fill(nCdcVert)
    nCdcVertVsnTr.Fill(nTracks, nCdcVert)
        
    
    # calc difference of CDC and CDC+TPC Vertex if there is exactly 1 Vertex found in each case
    if e.PndTpcCdcEvent.GetEntriesFast() == e.GFVertex.GetEntriesFast() and e.PndTpcCdcEvent.At(0).GetVertex().X()!=0 and e.PndTpcCdcEvent.At(0).GetVertex().Y()!=0 and e.PndTpcCdcEvent.At(0).GetVertex().Z()!=0 :
                
        DX.Fill(e.PndTpcCdcEvent.At(0).GetVertex().X() - e.GFVertex.At(0).getPos().X())
        DY.Fill(e.PndTpcCdcEvent.At(0).GetVertex().Y() - e.GFVertex.At(0).getPos().Y())
        DZ.Fill(e.PndTpcCdcEvent.At(0).GetVertex().Z() - e.GFVertex.At(0).getPos().Z())    
     

#map(analyse, tree)


c1=ROOT.TCanvas()
c1.Divide(3,3)

c1.cd(1)
CdcX.Draw()
CdcX.Fit("gaus")
c1.cd(2)
CdcXY.Draw("colz")
#CdcXY.Fit("xygaus")
c1.cd(3)
nTr.Draw()

c1.cd(4)
CdcY.Draw()
CdcY.Fit("gaus")
c1.cd(5)
CdcXZ.Draw("colz")
c1.cd(6)
CDCnVert.Draw()

c1.cd(7)
CdcZ.Draw()
CdcZ.Fit("gaus")
c1.cd(8)
CdcYZ.Draw("colz")
c1.cd(9)
nCdcVertVsnTr.Draw("colz")



c2=ROOT.TCanvas()
c2.Divide(3,3)

c2.cd(1)
CombX.Draw()
CombX.Fit("gaus")
c2.cd(2)
CombXY.Draw("colz")
c2.cd(3)
nTr.Draw()

c2.cd(4)
CombY.Draw()
CombY.Fit("gaus")
c2.cd(5)
CombXZ.Draw("colz")
c2.cd(6)
CombnVert.Draw()

c2.cd(7)
CombZ.Draw()
CombZ.Fit("gaus")
c2.cd(8)
CombYZ.Draw("colz")
c2.cd(9)
nCombVertVsnTr.Draw("colz")


c3=ROOT.TCanvas()
c3.Divide(1,3)

c3.cd(1)
DX.Draw()
DX.Fit("gaus")
c3.cd(2)
DY.Draw()
DY.Fit("gaus")
c3.cd(3)
DZ.Draw()
DZ.Fit("gaus")


input()

outfile.cd()
c1.Write()
c2.Write()
outfile.Close()
        

print "wrote out file"
print outname



#  ------------------------------- END OF ANA LOOP ---------------------------------------           



