
import ROOT, glob, math
from ROOT import std

dir = "/nfs/hicran/data/tpc/fopi/2010/decoded"


ROOT.gROOT.ProcessLine(".x rootlogon.C") 
ROOT.gROOT.ProcessLine('gSystem->Load("libPhysics")')
ROOT.gROOT.ProcessLine('gStyle->SetPalette(1)')

failed = ROOT.TH1D("Failed", "Failed Hits", 30,0,30)
recoMom = ROOT.TH1D("recoMom", "Rec. Momenta", 500,900,1100)
sampTimes = ROOT.TH1D("SamT", "Drift Time Distribution (samples)",511,0,511)

occXY = ROOT.TH2D("OccXY", "Cluster XY occupancy created from cosmic tracks",
                  200,-15,15,200,-15,15)
occZ = ROOT.TH1D("OccZ", "Cluster Z occupancy created from cosmic tracks",
                 200,0,75)

files = glob.glob(dir + "/*.reco.root")
files.sort()


# -------------------- driftVel correction --------------------------------
driftVelSim = 0.00055192   #(ArCo2 70/30 @ 250 V/cm)
driftVelReal = 0.00284     #(ArCo2 90/10 @ 350 V/cm)
velCorr = driftVelReal / driftVelSim

# define cuts in Z
cuts = (0,10,20,30,40,50,60)

resXs = dict([(i, ROOT.TH1D("StatsResX"+str(cuts[i]), 
                            "Cosmic Residuals X", 500,-1,1)) for i in range(len(cuts))])
resYs = dict([(i, ROOT.TH1D("StatsResY"+str(cuts[i]), 
                            "Cosmic Residuals Y", 500,-1,1)) for i in range(len(cuts))])
resZs = dict([(i, ROOT.TH1D("StatsResZ"+str(cuts[i]), 
                            "Cosmic Residuals Z", 500,-1,1)) for i in range(len(cuts))])

resXYs = dict([(i, ROOT.TH1D("StatsResXY"+str(cuts[i]), 
                             "Cosmic Residuals XY", 500,-1,1)) for i in range(len(cuts))])
resXsYs = dict([(i, ROOT.TH2D("StatsResXsYs"+str(cuts[i]), 
                             "Cosmic Residuals XY", 500,-0.5,0.5
                              ,500,-0.5,0.5)) for i in range(len(cuts))])

for f in range(10) :
    #list = file.split("_")
    #num = int(list[1][2:4])
    file = files[f]
    print(file)
    Rfile = ROOT.TFile(file, "read")
    tree = Rfile.Get("cbmsim")
    tree.SetBranchStatus("*", 0)
   #tree.SetBranchStatus("PndTpcSLResiduals.*", 1)
    tree.SetBranchStatus("TrackFitStat.*", 1)
    #tree.SetBranchStatus("PndTpcSample.*", 1)
    
    for e in tree :
        for tfs in e.TrackFitStat :
            for p in range(tfs.GetHitPositionsZ().size()) :
                z = tfs.GetHitPositionsZ().at(p) * velCorr
                if z > 1.7 :   #cut away noie in first 10 samples. DIRTY! temporary
                    for i in range(len(cuts)) :
                        if z < cuts[i] :
                            resXs[i-1].Fill(tfs.GetResX().at(p))
                            resYs[i-1].Fill(tfs.GetResY().at(p))
                            resZs[i-1].Fill(tfs.GetResZ().at(p))
                            resXYs[i-1].Fill(math.sqrt(tfs.GetResX().at(p)**2+tfs.GetResY().at(p)**2)) 
                            resXsYs[i-1].Fill(tfs.GetResX().at(p), tfs.GetResY().at(p)) 
                            break
                             
                    x = tfs.GetHitPositionsX().at(p)
                    y = tfs.GetHitPositionsY().at(p)
                    occXY.Fill(x,y)
                    occZ.Fill(z)
                    recoMom.Fill(tfs.GetP())
                
c1 = ROOT.TCanvas()
c1.Divide(3,2)
for i in range(6) :
    c1.cd(i+1)
    resXsYs[i].Draw("COLZ")
c2 = ROOT.TCanvas()
occXY.Draw("COLZ")
c3 = ROOT.TCanvas()
occZ.Draw()

diffX = ROOT.TGraph(6)
diffX.SetName("diffX")
diffX.SetTitle("X Resolution as function of Drift Length")

c4 = ROOT.TCanvas()
c4.Divide(3,2)
for i in range(6) :
    c4.cd(i+1)
    resXs[i].SetFillColor(ROOT.kAzure-8)
    resXs[i].Draw()
    testfit = ROOT.TF1("testfitX"+str(1),"gaus",-1,1)
    resXs[i].Fit(testfit, "N+", "", -1,1)
    
    fit = ROOT.TF1("fitfuncX"+str(1),"gaus + gaus(3)",-1,1)
    fit.SetNpx(1000)
    fit.SetParameter(0,testfit.GetParameter(0))
    fit.SetParLimits(0,testfit.GetParameter(0)*0.5, testfit.GetParameter(0)*2)
    fit.SetParameter(1,testfit.GetParameter(1))
    #fit.SetParLimits(1,testfit.GetParameter(1)*(-300), testfit.GetParameter(1)*300)
    fit.SetParameter(2,testfit.GetParameter(2))
    fit.SetParLimits(2,testfit.GetParameter(2)*0.5, testfit.GetParameter(2)*2)
    
    fit.SetParameter(3, 10)
    fit.SetParLimits(3, 0, testfit.GetParameter(0)/2)
    fit.SetParameter(4, 0)
    #fit.SetParLimits(4, testfit.GetParameter(1)*(-300), testfit.GetParameter(1)*300)
    fit.SetParameter(5, testfit.GetParameter(2))
    fit.SetParLimits(5, testfit.GetParameter(2), testfit.GetParameter(2)*30)
    resXs[i].Fit(fit, "+", "", -1,1)
    diffX.SetPoint(i,cuts[i]+5,fit.GetParameter(2))

c5 = ROOT.TCanvas()
c5.Divide(3,2)
for i in range(6) :
    c5.cd(i+1)
    resYs[i].SetFillColor(ROOT.kAzure-8)
    resYs[i].Draw()
    testfit = ROOT.TF1("testfitY"+str(1),"gaus",-1,1)
    resYs[i].Fit(testfit, "N+", "", -1,1)
    
    fit = ROOT.TF1("fitfuncY"+str(1),"gaus + gaus(3)",-1,1)
    fit.SetNpx(1000)
    fit.SetParameter(0,testfit.GetParameter(0))
    fit.SetParLimits(0,testfit.GetParameter(0)*0.5, testfit.GetParameter(0)*2)
    fit.SetParameter(1,testfit.GetParameter(1))
    #fit.SetParLimits(1,testfit.GetParameter(1)*(-300), testfit.GetParameter(1)*300)
    fit.SetParameter(2,testfit.GetParameter(2))
    fit.SetParLimits(2,testfit.GetParameter(2)*0.2, testfit.GetParameter(2)*5)
    
    fit.SetParameter(3, 10)
    fit.SetParLimits(3, 0, testfit.GetParameter(0)/2)
    fit.SetParameter(4, 0)
    #fit.SetParLimits(4, testfit.GetParameter(1)*(-300), testfit.GetParameter(1)*300)
    fit.SetParameter(5, testfit.GetParameter(2))
    fit.SetParLimits(5, testfit.GetParameter(2), testfit.GetParameter(2)*30)
    resYs[i].Fit(fit, "+", "", -1,1)


c6 = ROOT.TCanvas()
c6.Divide(3,2)
for i in range(6) :
    c6.cd(i+1)
    resXYs[i].SetFillColor(ROOT.kAzure-8)
    resXYs[i].Draw()

c7 = ROOT.TCanvas()
diffX.Draw("A*")
diffX.GetXaxis().SetTitle("Drift Length Z (cm)")


input()

