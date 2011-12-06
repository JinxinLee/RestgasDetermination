#!/usr/bin/python

import ROOT, glob, math, sys, os
from ROOT import std
from array import array

radCut = -1

def drawPrelim() :
    prelim= ROOT.TLatex()
    prelim.SetTextColor(ROOT.kGray)
    prelim.SetTextSize(0.07)
    prelim.SetTextAngle(20)
    prelim.SetNDC()
    prelim.DrawLatex(0.38,0.45,"preliminary")
    return

interactive = 0

#argument parsing:
for iarg in range(len(sys.argv)) :
    arg = sys.argv[iarg]
    if arg == "--prelim" :
        preliminary = 1
    if arg == "-f" :
        file = sys.argv[iarg+1]
    if arg == "-i" :
        interactive = 1
    if arg == "-radCut" :
        radCut = float(sys.argv[iarg+1])
        print "radCut set to ", radCut, "m"


        
ROOT.gROOT.ProcessLine(".L rootlogon.C") 
ROOT.gROOT.ProcessLine("rootlogon()") 
#ROOT.gROOT.ProcessLine(".L rootlogon_Bernhard.C") 
#ROOT.gROOT.ProcessLine("rootlogon_Bernhard()") 
#ROOT.gROOT.ProcessLine('gSystem->Load("libPhysics")')
#ROOT.gROOT.ProcessLine('gStyle->SetPalette(1)')
#pretty draw:
#ROOT.gROOT.ProcessLine('gROOT->SetStyle("col")') 
ROOT.gROOT.ProcessLine('gROOT->SetStyle("Plain")')


#  ---------------------------------------- ANA LOOP --------------------------------------

fcounter = 0
graphsX = {}
graphsY = {}
graphsXerr = {}
graphsYerr = {}

colors = [ROOT.kBlack, ROOT.kRed+1, ROOT.kAzure-3, ROOT.kTeal+3, ROOT.kOrange+2, ROOT.kGreen-4]


Rfile = ROOT.TFile.Open(file, "read")
tree = Rfile.Get("cdcTree")
#tree.SetBranchStatus("*", 0)
#tree.SetBranchStatus("TrackFitStat_0.*", 1)
#tree.SetBranchStatus("TrackFitStat.*", 1)

CDCdistX = ROOT.TH1D("CDCdistX", "CDC X hit distribution", 400,-120,120)
CDCdistX.GetXaxis().SetTitle("X (cm)")
CDCdistY = ROOT.TH1D("CDCdistY", "CDC Y hit distribution", 400,-120,120)
CDCdistY.GetXaxis().SetTitle("Y (cm)")

CDCTrackRad = ROOT.TH1D("CDCTrackRad", "CDC Track Radius Distribution", 100,0,30)
CDCTrackRad.GetXaxis().SetTitle("Track bending radius (m)")


nTracks = ROOT.TH1D("nTracks", "Number of CDC tracks", 10,0,10)
nTracks.GetXaxis().SetTitle("# of CDC tracks per event")

frame = ROOT.TH2D("frame", "",10,-120,120,10,-120,120)
frame.GetXaxis().SetTitle("X (cm)")
frame.GetYaxis().SetTitle("Y (cm)")


cTemp = ROOT.TCanvas()
totTracks = 0
   
for e in tree : 
    graphsX.clear()
    graphsY.clear()
    graphsXerr.clear()
    graphsYerr.clear()
    #for icdc in e.CdcHitBr :
    #    CDCdistX.Fill(icdc.GetHitPos().X())
    #    CDCdistY.Fill(icdc.GetHitPos().Y())
    trCounter=0    
    for icdctr in e.CdcTrackBr :
        radius = icdctr.GetRadius()
        CDCTrackRad.Fill(math.fabs(radius)/100.)
        # --------------------- CUT ON RADIUS --------------------------------
        if radius/100. < radCut and radCut > 0 :
            continue
        # --------------------------------------------------------------------
        if trCounter not in graphsX.keys() :
            graphsX[trCounter] = []
            graphsY[trCounter] = []
            graphsXerr[trCounter] = []
            graphsYerr[trCounter] = []
        for ipitr in range(icdctr.GetNpoints()) :
            ipt = e.CdcHitBr.At(icdctr.GetPointID(ipitr))
            CDCdistX.Fill(ipt.GetHitPos().X())
            CDCdistY.Fill(ipt.GetHitPos().Y())
            graphsX[trCounter].append(ipt.GetHitPos().X())
            graphsY[trCounter].append(ipt.GetHitPos().Y())
            graphsXerr[trCounter].append(ipt.GetXposErr().X())
            graphsYerr[trCounter].append(ipt.GetYposErr().Y())
            
        trCounter += 1
    
    nTracks.Fill(trCounter)   
    totTracks += trCounter
  
    graphs = []
    cTemp.cd()
    
    frame.Draw()
    frame.SetStats(0)
    cTemp.Update()
   
    
    for itr in range(trCounter) :
        if itr >= len(colors) :  #dirty, remove later
            continue
        graphs.append(ROOT.TGraphErrors(len(graphsX[itr]),
                                        array('d',graphsX[itr]),
                                        array('d',graphsY[itr]),
                                        array('d',graphsXerr[itr]),
                                        array('d',graphsYerr[itr])))
        
        graphs[len(graphs)-1].SetLineColor(colors[itr])
        graphs[len(graphs)-1].SetMarkerColor(colors[itr])
        graphs[len(graphs)-1].SetHistogram(ROOT.TH2D(frame))
        
        if itr == 0 :
            graphs[len(graphs)-1].Draw("ALP")
            #graphs[len(graphs)-1].GetHistogram().GetXaxis().SetRangeUser(-120,120)
            #graphs[len(graphs)-1].GetHistogram().GetYaxis().SetRangeUser(-120,120)
        else :
            graphs[len(graphs)-1].Draw("LP")
    
    if interactive :        
        cTemp.Update()            
        try :
            input()
        except :
            print "next one .."

c1 = ROOT.TCanvas()
c1.Divide(4,1)
c1.cd(1)
CDCdistX.Draw()
c1.cd(2)
CDCdistY.Draw()
c1.cd(3)
CDCTrackRad.Draw()
if radCut > 0:
    ymin = 0
    ymax = CDCTrackRad.GetMaximum()
    cutLine = ROOT.TLine(radCut,ymin,radCut,ymax)
    cutLine.SetLineColor(ROOT.kRed+1)
    cutLine.SetLineWidth(2)
    cutLine.Draw()

c1.cd(4)
nTracks.Draw()

try :
    input()
except :
    print "Closing ..." 

print "File had ", totTracks, " tracks meeting the criteria"

Rfile.Close()


