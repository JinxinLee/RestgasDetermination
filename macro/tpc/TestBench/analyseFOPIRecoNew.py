import ROOT, glob, math, sys, os
from ROOT import std
from array import array

def drawPrelim() :
    prelim= ROOT.TLatex()
    prelim.SetTextColor(ROOT.kGray)
    prelim.SetTextSize(0.07)
    prelim.SetTextAngle(20)
    prelim.SetNDC()
    prelim.DrawLatex(0.38,0.45,"preliminary")
    return

preliminary = 0
chi2ProbCut = 1.
numFiles = 10000000
processClusters = 1
processTracks = 1

dir = "/nfs/hicran/data/tpc/fopi/2011/reconstructed"
runs = ""


#argument parsing:
for iarg in range(len(sys.argv)) :
    arg = sys.argv[iarg]
    if arg == "--prelim" :
        preliminary = 1
    if arg == "-chi2ProbCut" :
        chi2ProbCut = float(sys.argv[iarg+1])
        print "chi2ProbCut = %f" % chi2ProbCut
    if arg == "-path" :
        dir = sys.argv[iarg+1];
    if arg == "-n" :
        numFiles = int(sys.argv[iarg+1])
    if arg == "-noCl" :     #ignore cluster branch and associates
        processClusters = 0
        print "**** Disabled cluster analysis & plots *****"
    if arg == "-noTr" :
        processTracks = 0
    if arg == "-runs" :
        runs = sys.argv[iarg+1];    
        
dashIndex = runs.find("-")
if dashIndex > 0 :
    runs = runs.split("-")
    runList = range(int(runs[0]), int(runs[1])+1)
if dashIndex < 0 and len(runs) > 1 :
    runList = [runs]

outfile = ROOT.TFile("anaOut.root", "recreate")

ROOT.gROOT.ProcessLine(".L rootlogon.C") 
ROOT.gROOT.ProcessLine("rootlogon()") 
ROOT.gROOT.ProcessLine(".L rootlogon_Bernhard.C") 
ROOT.gROOT.ProcessLine("rootlogon_Bernhard()") 
#ROOT.gROOT.ProcessLine('gSystem->Load("libPhysics")')
#ROOT.gROOT.ProcessLine('gStyle->SetPalette(1)')
#pretty draw:
ROOT.gROOT.ProcessLine('gROOT->SetStyle("col")') 
ROOT.gROOT.ProcessLine('gROOT->SetStyle("Plain")')


failed = ROOT.TH1D("Failed", "Failed Hits", 30,0,30)
recoMom = ROOT.TH1D("recoMom", "Rec. Momenta", 500,900,1100)
sampTimes = ROOT.TH1D("SamT", "Drift Time Distribution (samples)",511,0,511)

occXY = ROOT.TH2D("OccXY", "Cluster XY occupancy created from tracks",
                  200,-15,15,200,-15,15)
occXY_end = ROOT.TH2D("OccXY_end", 
                      "Cluster XY (z>-10cm) occupancy created from tracks",
                      200,-15,15,200,-15,15)
occXY_amp = ROOT.TH2D("OccXY_amp", 
                      "Cluster XY occupancy weighted with Amp",
                      200,-15,15,200,-15,15)
occXY_end_amp = ROOT.TH2D("OccXY_end_amp", 
                          "Cluster XY (z>-10cm) occupancy weighted with Amp",
                          200,-15,15,200,-15,15)
occZ = ROOT.TH1D("OccZ", "Cluster Z occupancy created from tracks",
                 200,-65,15)

files = glob.glob(dir + "/*.reco.root")
files.sort()


# -------------------- driftVel correction --------------------------------
#driftVelSim = 0.00055192   #(ArCo2 70/30 @ 250 V/cm)
#driftVelReal = 0.00284     #(ArCo2 90/10 @ 350 V/cm)
#velCorr = driftVelReal / driftVelSim

# define cuts in Z
zCuts = (-60,-50,-40,-30,-20,-10,0)
#define cuts in Size
sCuts = (0,1,2,3,5,10,20)

# define offset in Z 1 mu s * 2.8
OFFSET = -2.8

resUs = dict([(i, ROOT.TH1D("StatsResX"+str(zCuts[i]), 
                            "Residuals U (Z')", 500,-1,1)) 
              for i in range(len(zCuts))])
pullsUs = dict([(i, ROOT.TH1D("StatsPullX"+str(zCuts[i]), 
                            "Pulls U (Z')", 100,-3,3)) 
              for i in range(len(zCuts))])


resVIDs = dict([(i, ROOT.TH1D("StatsResVID"+str(zCuts[i]), 
                              "Residuals V (X') single track events", 
                              500,-1,1)) 
                for i in range(len(zCuts))])
resVs = dict([(i, ROOT.TH1D("StatsResV"+str(zCuts[i]), 
                            "Residuals V (X')", 500,-1,1)) 
              for i in range(len(zCuts))])


resVvsZvsS = dict([(i, dict([(j, ROOT.TH1D("resVvsZvsS"+str(i)+str(j),
                                           "Residuals V (X') Z>"+str(zCuts[i])
                                           +"ClSize>"+str(sCuts[j]),500,-1,1,))
                             for j in range(len(sCuts))]))
                   for i in range(len(zCuts))])
                                         

pullsVs = dict([(i, ROOT.TH1D("StatsPullV"+str(zCuts[i]), 
                            "Pulls V (X')", 100,-3,3)) 
              for i in range(len(zCuts))])

cl2Ds = dict([(i, ROOT.TH1D("cl2Ds"+str(zCuts[i]), 
                            "Cluster Size Distributions as Function of Z", 50,0,50)) 
              for i in range(len(zCuts))])

clSvA = dict([(i, ROOT.TH1D("clsVa"+str(i), "Amplitude Distribution for ClSize>"
                            +str(sCuts[i]),500,0,1000)) for i in range(len(sCuts))])


#resUVs = dict([(i, ROOT.TH1D("StatsResXY"+str(zCuts[i]), 
#                             "Residuals X'Y'", 500,-1,1)) for i in range(len(zCuts))])
resVsUs = dict([(i, ROOT.TH2D("StatsResXsYs"+str(zCuts[i]), 
                              "Residuals U(Z') vs V(X')", 500,-0.5,0.5,
                              500,-0.5,0.5)) 
                for i in range(len(zCuts))])

clSize2D = ROOT.TH1D("clSize2D", "2D Cluster Size Distribution", 100,0,100)
clSize2D.SetFillColor(ROOT.kBlack)
clSize = ROOT.TH1D("clSize", "Cluster Size Distribution", 100,0,100)
clSize.SetLineColor(ROOT.kRed+3)
clSizevsDrift = ROOT.TH2D("clnSizevsDrift", "Cluster Size vs. Drift",
                          100,-65,15,60,0,60)
clOccRbins = 50
clOccZbins = 200
clOccRmin = 5
clOccRmax = 15
clOccZmin = -65
clOccZmax = 15
clOcc = ROOT.TH2D("clOcc", "Cluster Chamber Occupancy Z-R",
                  clOccZbins, clOccZmin, clOccZmax,
                  clOccRbins, clOccRmin, clOccRmax)
clOccAmp = ROOT.TH2D("clOccAmp",
                     "Cluster Chamber Occupancy Z-R (weighted with amp)",
                  clOccZbins, clOccZmin, clOccZmax,
                  clOccRbins, clOccRmin, clOccRmax)

clOccPR = ROOT.TH2D("clOccPR", "Track Cluster Chamber Occupancy Z-R",
                    clOccZbins, clOccZmin, clOccZmax,
                    clOccRbins, clOccRmin, clOccRmax)
clOccPRAmp = ROOT.TH2D("clOccPRAmp", "Track Cluster Chamber Occupancy Z-R (weighted with amp)",
                       clOccZbins, clOccZmin, clOccZmax,
                       clOccRbins, clOccRmin, clOccRmax)

chi2prob = ROOT.TH1D("chi2prob", "ChiSqu Probability Distribution", 500,0,1)
chi2probID = ROOT.TH1D("chi2probID", "ChiSqu Probability Distribution", 500,0,1)
chi2probID.SetLineColor(ROOT.kRed+2)
chi2raw = ROOT.TH1D("chi2raw", "ChiSqu2 / NDF", 300,0,10)

phiDist = ROOT.TH1D("phiDist", "Phi Distribution of fitted Tracks", 300,-3.5,3.5)
thetaDist = ROOT.TH1D("thetaDist", "Theta Distribution of fitted Tracks", 300,-3.5,3.5)

sigxDist = ROOT.TH1D("sigxDist", "Cluster SigmaX Distribution", 500, 0, 0.3)
sigyDist = ROOT.TH1D("sigyDist", "Cluster SigmaY Distribution", 500, 0, 0.3)
sigXvsClSize = ROOT.TH2D("sigXClSize", "Cluster SigmaX vs Cluster Size",
                         80,0,40,500,0,0.3)
sigXvsDrift = ROOT.TH2D("sigXvsZ", "Cluster SigmaX vs Drift Length Z",
                         400,0,65,400,0,3000)
sigXvsDrift.GetYaxis().SetTitle("#sigma_X (#mu m)")

nTracksDist = ROOT.TH1D("nTracksDist", "Distribution of track multiplicity",50,0,50)

pullAllV = ROOT.TH1D("pullAllV", "V (X') Pull Distribution", 200,-3,3)

DIFFX = 0.0227*10000 #(mu m/ mu s)

diffT = ROOT.TF1("diffT","[0]*TMath::Sqrt(x)",0,75)
diffT.SetNpx(1000)
diffT.SetLineWidth(2)
diffT.SetLineStyle(4)
diffT.SetParameter(0,DIFFX)

resDX = ROOT.TF1("resDX", "TMath::Sqrt([0]/12 + [1]*x)", 0, 75)
resDX.SetNpx(1000)
resDX.SetParameter(0,3000**2)
resDX.SetParameter(1,DIFFX**2)
resDX.SetLineWidth(2)


chi2func = ROOT.TF1("meh", "[0]*x*TMath::Exp(-2*x)",0,10)
chi2func.SetParameter(0,2500)


probVsZ = ROOT.TH2D("probVsZ", "#xi^2 Probability vs Z (rough)",
                    200,-65,15,200,0,1)
probVsNHits = ROOT.TH2D("probVNHits", "#xi^2 Probability vs number of track hits",
                        200,0,100,200,0,1)


#  ---------------------------------------- ANA LOOP --------------------------------------

fcounter = 0

simpath=""
for env in os.environ.keys() :
    if env == "PANDAPATH" and len(os.environ[env]) > 0 :
        simpath = os.environ[env]
        print "Setting pandaroot path to %s" %simpath

poolfile = simpath + "/tpc/TestBench/PadShapePrototype.dat"
pool = ROOT.PndTpcPadShapePool(poolfile)
planefile = simpath + "/tpc/TestBench/padPlane_FOPI.dat"
plane = ROOT.PndTpcPadPlane(planefile, pool)

for file in files :
    #check if it appears in the runList
    if len(runList) >= 1 :
        found = 0
        for i in runList :
            if file.find(str(i)) >= 0 :
                found = 1
        if found == 0 :
            continue
    
    if fcounter > numFiles :
        continue
    
    if file.find("run") < 0 :
        continue
    
    fcounter+=1
    print(file)
    Rfile = ROOT.TFile.Open(file, "read")
    #print(Rfile.GetOpenTimeout())
    tree = Rfile.Get("cbmsim")
    tree.SetBranchStatus("*", 0)
    tree.SetBranchStatus("TrackFitStat_0.*", 1)
    if processClusters :
        tree.SetBranchStatus("PndTpcCluster.*", 1)
    #if processTracks :
     #   tree.SetBranchStatus("TrackPostFit.*", 1)
    
    counter = 0

    for e in tree :
        if processClusters :
            for cl in e.PndTpcCluster :
                sig = cl.sig()
                sigxDist.Fill(sig.X())
                sigyDist.Fill(sig.Y())
                size = cl.size()
                pos = cl.pos()
                amp = cl.amp()
                rad = math.sqrt(pos.X()**2 + pos.Y()**2)
                sigXvsClSize.Fill(size, sig.X())
                sigXvsDrift.Fill(pos.Z()+62.5, sig.X()*10000)
                clSizevsDrift.Fill(pos.Z(),size)
                clOcc.Fill(pos.Z(), rad)
                clOccAmp.Fill(pos.Z(), rad, amp)
               
        nTracks = e.TrackFitStat_0.GetEntriesFast()  
        if nTracks < 1 :
            continue
            
        nTracksDist.Fill(nTracks)
            
                    
        for tfs in e.TrackFitStat_0 :
            chi2 = tfs.getChi2()
            #redChi2 = tfs.getRedChi2()
            NDF = tfs.getNDF()
            NDIM=4
            
            numHits = tfs.GetHitPositionsZ().size()
            
            if numHits < 1 :
                continue
            
            chi2Prob = ROOT.TMath.Prob(chi2, NDF)
            
            if chi2Prob > chi2ProbCut :
                continue
        
            chi2prob.Fill(chi2Prob)
            if NDF > 0 :
                chi2raw.Fill(chi2/(NDF))
            else : 
                chi2raw.Fill(1000)
        
            probVsZ.Fill(tfs.GetHitPositionsZ().at(0),chi2Prob)
            probVsNHits.Fill(numHits,chi2Prob)
            
            if nTracks == 1:
                chi2probID.Fill(chi2Prob)
                
            mom = tfs.GetMom()
            mom.SetMag(1.)
            phiDist.Fill(mom.Phi())
            thetaDist.Fill(mom.Theta())
            
            #build orthogonal vector to fix plane - res x mom
            vecX = ROOT.TVector3(1,0,0)
            vecZ = ROOT.TVector3(0,0,1)
            #define new coordinate system for this track
            u = mom.Cross(vecX)
            v = mom.Cross(u)
        
            shout = 1
            thetaDeg = mom.Theta()*180/(math.pi)
            
            for p in range(numHits) :
                z = tfs.GetHitPositionsZ().at(p)
                x = tfs.GetHitPositionsX().at(p)
                y = tfs.GetHitPositionsY().at(p)
                
                clAmp = tfs.GetAmps().at(p)
            
            #look for tracks parallel to readout at end of chamber
                if shout and z > 56 and thetaDeg>88 and thetaDeg<92 :
                    shout=0
                    print "Found track at end of chamber: Ev %i in file" %counter
                
                xyRad = math.sqrt(x**2 + y**2)
            
                clOccPR.Fill(z,xyRad)
                clOccPRAmp.Fill(z,xyRad,clAmp)                
            
                resX = tfs.GetResX().at(p)
                resY = tfs.GetResY().at(p)
                resZ = tfs.GetResZ().at(p)
                res = ROOT.TVector3(resX,resY,resZ)
            
                sigX = tfs.GetSigX().at(p)
                sigY = tfs.GetSigY().at(p)
                sigZ = tfs.GetSigZ().at(p)
                sig = ROOT.TVector3(sigX,sigY,sigZ)
            
                resU = tfs.GetResU().at(p)
                resV = tfs.GetResV().at(p)   
                
                # todo: 
                sigU = sig.Dot(u)
                sigV = sig.Dot(v)   
            
                cl2Dsize = tfs.Get2DClSizes().at(p)
                clTotSize = tfs.GetClSizes().at(p)
            
                clSize2D.Fill(cl2Dsize)
                clSize.Fill(clTotSize)
            
                for j in range(len(sCuts)):
                    if cl2Dsize <= sCuts[j] :
                        clSvA[j].Fill(clAmp)
                        break
                
                for i in range(len(zCuts)) :
                    if z < zCuts[i] :
                        cl2Ds[i].Fill(cl2Dsize)
                        
                        resUs[i].Fill(resU)
                        resVs[i].Fill(resV)
                
                        pullsUs[i].Fill(resU/sigU)
                        pullsVs[i].Fill(resV/sigV)
                
                        #resUVs[i].Fill(math.sqrt(resX**2 + resY**2)) 
                        resVsUs[i].Fill(resV, resU)
                        
                        #compare with these filters:
                        if nTracks == 1 :
                            resVIDs[i].Fill(resV)
                        
                            #if xyRad > 8 and xyRad < 12:
                            #if numHits > 20 :
                            for j in range(len(sCuts)) :
                                if cl2Dsize <= sCuts[j] :
                                    resVvsZvsS[i][j].Fill(resV)
                                    break
                            
                            break

                    
                x = tfs.GetHitPositionsX().at(p)
                y = tfs.GetHitPositionsY().at(p)
                occXY.Fill(x,y)
                occXY_amp.Fill(x,y,clAmp)
                if z > -10 :
                    occXY_end.Fill(x,y)
                    occXY_end_amp.Fill(x,y,clAmp)
                occZ.Fill(z)
                recoMom.Fill(tfs.GetP())
                
        counter+=1    
#  ------------------------------- END OF ANA LOOP ---------------------------------------           

    
outfile.cd()

c1 = ROOT.TCanvas()
c1.SetTitle(dir)
c1.Divide(3,2)
for i in range(6) :
    c1.cd(i+1)
    resVsUs[i].GetXaxis().SetTitle("Residual V (cm)")
    resVsUs[i].GetYaxis().SetTitle("Residual U (cm)")
    resVsUs[i].Draw("COLZ")
    # preliminary
    if preliminary :
        drawPrelim()
    resVsUs[i].Write()

c2 = ROOT.TCanvas()
c2.SetTitle(dir)
c2.Divide(2,2)
c2.cd(1).SetLogz(1)
occXY.Draw("COLZ")
if preliminary:
    drawPrelim()
occXY.Write()
c2.cd(2).SetLogz(1)
occXY_end.Draw("COLZ")
if preliminary:
    drawPrelim()
occXY_end.Write()
c2.cd(3).SetLogz(1)
occXY_amp.Draw("COLZ")
if preliminary:
    drawPrelim()
occXY_amp.Write()
c2.cd(4).SetLogz(1)
occXY_end_amp.Draw("COLZ")
if preliminary:
    drawPrelim()
occXY_end_amp.Write()


occXY_end.Write()

c3 = ROOT.TCanvas()
c3.SetTitle(dir)
occZ.Draw()
occZ.Write()

bckgrShare = ROOT.TGraph(6)
bckgrShare.SetName("bckgShare")
bckgrShare.SetTitle("Share of background (from fits)")
bckgrShareID = ROOT.TGraph(6)
bckgrShareID.SetName("bckgShare")
bckgrShareID.SetTitle("Share of background (one track events)")

                            

c4 = ROOT.TCanvas()
c4.SetTitle(dir)
c4.Divide(3,2)
for i in range(6) :
    c4.cd(i+1)
    resVs[i].SetFillColor(ROOT.kAzure-8)
    resVs[i].GetXaxis().SetTitle("Residual V (cm)")
    resVs[i].Draw()
    testfit = ROOT.TF1("testfitX"+str(1),"gaus",-1,1)
    resVs[i].Fit(testfit, "N+", "", -1,1)
    
    fit = ROOT.TF1("fitfuncX"+str(1),"gaus + gaus(3)",-1,1)
    fit.SetNpx(1000)
    fit.SetParameter(0,testfit.GetParameter(0))
    fit.SetParLimits(0,testfit.GetParameter(0)*0.2, testfit.GetParameter(0)*5)
    fit.SetParameter(1,testfit.GetParameter(1))
    fit.SetParLimits(1,testfit.GetParameter(1)-0.08, testfit.GetParameter(1)+0.08)
    fit.SetParameter(2,testfit.GetParameter(2))
    fit.SetParLimits(2,testfit.GetParameter(2)*0.1, testfit.GetParameter(2)*20)
    
    fit.SetParameter(3, 100)
    fit.SetParLimits(3, 0, testfit.GetParameter(0)/3.)
    fit.SetParameter(4, 0)
    fit.SetParLimits(4, testfit.GetParameter(1)-0.08, testfit.GetParameter(1)+0.08)
    fit.SetParameter(5, testfit.GetParameter(2))
    fit.SetParLimits(5, testfit.GetParameter(2)*2, testfit.GetParameter(2)*30)
    resVs[i].Fit(fit, "+", "", -1,1)
    #diffV.SetPoint(i,zCuts[i]+5,fit.GetParameter(2)*10000)

    # preliminary
    if preliminary :
        drawPrelim()

    resVs[i].Write()
    #calculate ratio of central and background integrals
    int1 = fit.GetParameter(0)*fit.GetParameter(2)
    int2 = fit.GetParameter(3)*fit.GetParameter(5)
    ratio = 0
    if int1 > 0 :
        ratio = int2/int1
    bckgrShare.SetPoint(i,zCuts[i]+5,ratio)
                          

c5 = ROOT.TCanvas()
c5.SetTitle(dir)
c5.Divide(3,2)
for i in range(6) :
    c5.cd(i+1)
    ROOT.gPad.SetLogy(1)
    cl2Ds[i].Draw()


c6 = ROOT.TCanvas()
c6.SetTitle(dir)
c6.Divide(3,2)
for i in range(6) :
    c6.cd(i+1)
    resUs[i].SetFillColor(ROOT.kAzure-8)
    resUs[i].GetXaxis().SetTitle("Residual U (cm)")
    resUs[i].Draw()
    
    testfit = ROOT.TF1("testfitY"+str(1),"gaus",-1,1)
    resUs[i].Fit(testfit, "N+", "", -1,1)
    
    fit = ROOT.TF1("fitfuncY"+str(1),"gaus + gaus(3)",-1,1)
    fit.SetNpx(1000)
    fit.SetParameter(0,testfit.GetParameter(0))
    fit.SetParLimits(0,testfit.GetParameter(0)*0.5, testfit.GetParameter(0)*2)
    fit.SetParameter(1,testfit.GetParameter(1))
    fit.SetParLimits(1,testfit.GetParameter(1)-0.03, testfit.GetParameter(1)+0.03)
    fit.SetParameter(2,testfit.GetParameter(2))
    fit.SetParLimits(2,testfit.GetParameter(2)*0.2, testfit.GetParameter(2))
    
    fit.SetParameter(3, 10)
    fit.SetParLimits(3, 0, testfit.GetParameter(0)/1.5)
    fit.SetParameter(4, 0)
    fit.SetParLimits(4, testfit.GetParameter(1)-0.03, testfit.GetParameter(1)+0.03)
    fit.SetParameter(5, testfit.GetParameter(2)*3)
    fit.SetParLimits(5, testfit.GetParameter(2)*1.4, testfit.GetParameter(2)*20)
    resUs[i].Fit(fit, "+", "", -1,1)

    # preliminary
    if preliminary :
       drawPrelim()

    resUs[i].Write()

c7 = ROOT.TCanvas()
c7.SetTitle(dir)
c7.Divide(3,2)

drifts = []
vals = []
valsID = []
errX = []
errY = []
errYID = []


for i in range(6) :
    c7.cd(i+1)
    resVIDs[i].SetFillColor(ROOT.kAzure-8)
    resVIDs[i].GetXaxis().SetTitle("Residual V (cm)")
    resVIDs[i].Draw()
    testfit = ROOT.TF1("testfitX"+str(1),"gaus",-1,1)
    resVIDs[i].Fit(testfit, "N+", "", -1,1)

    fit = ROOT.TF1("fitfuncX"+str(1),"gaus + gaus(3)",-1,1)
    fit.SetNpx(1000)
    fit.SetParameter(0,testfit.GetParameter(0))
    fit.SetParLimits(0,testfit.GetParameter(0)*0.2, testfit.GetParameter(0)*5)
    fit.SetParameter(1,testfit.GetParameter(1))
    fit.SetParLimits(1,testfit.GetParameter(1)-0.08, testfit.GetParameter(1)+0.08)
    fit.SetParameter(2,testfit.GetParameter(2))
    fit.SetParLimits(2,testfit.GetParameter(2)*0.1, testfit.GetParameter(2)*20)
    
    fit.SetParameter(3, 100)
    fit.SetParLimits(3, 0, testfit.GetParameter(0)/3.)
    fit.SetParameter(4, 0)
    fit.SetParLimits(4, testfit.GetParameter(1)-0.08, testfit.GetParameter(1)+0.08)
    fit.SetParameter(5, testfit.GetParameter(2))
    fit.SetParLimits(5, testfit.GetParameter(2)*2, testfit.GetParameter(2)*30)

    resVIDs[i].Fit(fit, "+", "", -1,1)

    # preliminary
    if preliminary :
       drawPrelim()
        
    #diffVID.SetPoint(i,zCuts[i]+5,fit.GetParameter(2)*10000)
    #calculate ratio of central and background integrals
    int1 = fit.GetParameter(0)*fit.GetParameter(2)
    int2 = fit.GetParameter(3)*fit.GetParameter(5)
    ratio = int2/int1
    #mean resolution:
    meanRes = (int1*fit.GetParameter(2)+int2*fit.GetParameter(5))/(int1+int2)
    #diffV.SetPoint(i,zCuts[i]+5,meanRes*10000)

    drifts.append(zCuts[i] + 5)   #ugly, half the binwidth
    valsID.append(fit.GetParameter(2)*10000)
    errYID.append(math.sqrt(fit.GetParError(2)**2 + fit.GetParError(1)**2)*10000)
    vals.append(meanRes*10000)
    errY.append(errYID[i])
    errX.append(0)
    
    bckgrShareID.SetPoint(i,zCuts[i]+5,ratio)
    
    resVIDs[i].Write()

c8 = ROOT.TCanvas()
c8.SetTitle(dir)
bckgrShare.SetMarkerStyle(21)
bckgrShare.GetYaxis().SetRangeUser(0,1)
bckgrShare.GetXaxis().SetTitle("Drift Length Z (cm)")
bckgrShare.Draw("ALP")
bckgrShare.Write()
bckgrShareID.SetMarkerStyle(21)
bckgrShareID.SetMarkerColor(ROOT.kRed+2)
bckgrShareID.GetYaxis().SetRangeUser(0,1)
bckgrShareID.Draw("LP")
bckgrShareID.Write()


diffV = ROOT.TGraphErrors(6,array('d',drifts), array('d',vals), array('d',errX), array('d',errY))
diffV.SetName("diffX")
diffV.SetTitle("V(X') Resolution as function of Drift Length")
diffVID = ROOT.TGraphErrors(6,array('d',drifts), array('d',valsID), array('d',errX), array('d',errYID))
diffVID.SetMarkerColor(ROOT.kRed+2)


c9 = ROOT.TCanvas()
c9.SetTitle(dir)
diffV.SetMarkerStyle(21)
diffV.Draw("ALP")
diffV.GetXaxis().SetTitle("Drift Length Z (cm)")
diffV.GetYaxis().SetTitle("X Resolution (#mu m)")
diffV.GetYaxis().SetRangeUser(0,2000)
diffVID.SetMarkerStyle(21)
diffVID.Draw("LP")
diffV.Write()
diffVID.Write()
diffT.Draw("same")
# preliminary
if preliminary :
  drawPrelim()

if processClusters :
    c10 = ROOT.TCanvas()
    c10.SetTitle(dir)
    c10.Divide(2,1)
    c10.cd(1)
    clSize2D.Draw()
    clSize.Draw("same")
    # preliminary
    if preliminary :
        drawPrelim()

    c10.cd(2).SetLogz(1)
    clSizevsDrift.Draw("colz")
    # preliminary
    if preliminary :
        drawPrelim()

c11 = ROOT.TCanvas()
c11.SetTitle(dir)
c11.cd()
ROOT.gPad.SetLogy(1)
chi2prob.Draw()
chi2probID.Draw("same")
# preliminary
if preliminary :
    drawPrelim()


c12 = ROOT.TCanvas()
c12.SetTitle(dir)
chi2raw.Draw()
chi2func.SetNpx(1000)
# preliminary
if preliminary :
    drawPrelim()

#chi2func.Draw("same")

c13 = ROOT.TCanvas()
c13.SetTitle(dir)
c13.Divide(2,1)
c13.cd(1)
phiDist.Draw()
c13.cd(2)
thetaDist.Draw()

c14 = ROOT.TCanvas()
c14.SetTitle(dir)
c14.Divide(2,1)
c14.cd(1)
sigxDist.Draw()
# preliminary
if preliminary :
   drawPrelim()


c14.cd(2)
sigyDist.Draw()
# preliminary
if preliminary :
    drawPrelim()

if processClusters :
    c15 = ROOT.TCanvas()
    c15.SetTitle(dir)
    c15.Divide(2,1)
    c15.cd(1).SetLogz()
    sigXvsClSize.Draw("colz")
    # preliminary
    if preliminary :
        drawPrelim()

    c15.cd(2).SetLogz()
    sigXvsDrift.Draw("colz")
    diffT.Draw("same")
    #resDX.Draw("same")
    # preliminary
    if preliminary :
        drawPrelim()


c16 = ROOT.TCanvas()
c16.SetTitle(dir)
nTracksDist.Draw()


c17 = ROOT.TCanvas()
c17.SetTitle(dir)
ROOT.gStyle.SetOptFit(1111)
c17.Divide(3,2)

for i in range(6) :
    c17.cd(i+1)
    pullsVs[i].SetFillColor(ROOT.kAzure-8)
    pullsVs[i].GetXaxis().SetTitle("Pulls V")
    pullsVs[i].Draw()

    testfit = ROOT.TF1("testfitX"+str(1),"gaus",-3,3)
    pullsVs[i].Fit(testfit, "N+", "", -3,3)
    
    fit = ROOT.TF1("fitfuncX"+str(1),"gaus + gaus(3)",-3,3)
    fit.SetNpx(1000)
    fit.SetParameter(0,testfit.GetParameter(0))
    fit.SetParLimits(0,testfit.GetParameter(0)*0.2, testfit.GetParameter(0)*5)
    fit.SetParameter(1,testfit.GetParameter(1))
    fit.SetParLimits(1,testfit.GetParameter(1)-0.08, testfit.GetParameter(1)+0.08)
    fit.SetParameter(2,testfit.GetParameter(2))
    fit.SetParLimits(2,testfit.GetParameter(2)*0.1, testfit.GetParameter(2)*20)
    
    fit.SetParameter(3, 100)
    fit.SetParLimits(3, 0, testfit.GetParameter(0)/4.)
    fit.SetParameter(4, 0)
    fit.SetParLimits(4, testfit.GetParameter(1)-0.08, testfit.GetParameter(1)+0.08)
    fit.SetParameter(5, testfit.GetParameter(2))
    fit.SetParLimits(5, testfit.GetParameter(2), testfit.GetParameter(2)*30)
    pullsVs[i].Fit(fit, "+", "", -3,3)

    # preliminary
    if preliminary :
        drawPrelim()
        
    pullsVs[i].Write()

#c17.Update()

c18 = ROOT.TCanvas()
c18.SetTitle(dir)
c18.Divide(len(sCuts)-1, len(zCuts)-1)
for i in range(len(zCuts)-1) :
    for j in range(len(sCuts)-1) :
        c18.cd(i*(len(zCuts)-1)+j+1)
        resVvsZvsS[i][j].SetFillColor(ROOT.kSpring+5)
        resVvsZvsS[i][j].Draw()
        drawPrelim()
        resVvsZvsS[i][j].Write()


if processClusters :
    c19 = ROOT.TCanvas()
    c19.SetTitle(dir)
    c19.Divide(2,2)

    #properly normalize
    rBinWidth = (clOccRmax-clOccRmin)/float(clOccRbins)
    for z in range(clOccZbins):
        for r in range(clOccRbins) :
            cont = clOcc.GetBinContent(z+1,r+1)
            rad = r*rBinWidth + clOccRmin
            R1 = rad - 0.5*rBinWidth
            R2 = rad + 0.5*rBinWidth
            clOcc.SetBinContent(z+1,r+1,cont/(R2**2 - R1**2))
            cont = clOccAmp.GetBinContent(z+1,r+1)
            clOccAmp.SetBinContent(z+1,r+1,cont/(R2**2 - R1**2))
            cont = clOccPR.GetBinContent(z+1,r+1)
            clOccPR.SetBinContent(z+1,r+1,cont/(R2**2 - R1**2))
            cont = clOccPRAmp.GetBinContent(z+1,r+1)
            clOccPRAmp.SetBinContent(z+1,r+1,cont/(R2**2 - R1**2))
    c19.cd(1)
    clOcc.Draw("colz")
    if preliminary:
        drawPrelim()
    clOcc.Write()
    c19.cd(2)
    clOccAmp.Draw("colz")
    if preliminary:
        drawPrelim()
    clOccAmp.Write()
    c19.cd(3)
    clOccPR.Draw("colz")
    if preliminary:
        drawPrelim()
    clOccPR.Write()
    c19.cd(4)
    clOccPRAmp.Draw("colz")
    if preliminary:
        drawPrelim()
    clOccPRAmp.Write()


c20 = ROOT.TCanvas()
c20.SetTitle(dir)
c20.Divide(len(sCuts),1)
for i in range(len(sCuts)) :
    c20.cd(i+1)
    clSvA[i].Draw()
    if preliminary:
        drawPrelim()
    clSvA[i].Write()

c21 = ROOT.TCanvas()
c21.SetTitle(dir)
c21.Divide(2,1)
c21.cd(1)
probVsZ.Draw()
probVsZ.Write()
c21.cd(2)
probVsNHits.Draw()
probVsNHits.Write()


input()

outfile.Close()


