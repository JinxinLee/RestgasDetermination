
import ROOT, glob, math
from ROOT import std

dir = "/nfs/hicran/data/tpc/fopi/2010/decoded"

outfile = ROOT.TFile("anaOut.root", "recreate")

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
#driftVelSim = 0.00055192   #(ArCo2 70/30 @ 250 V/cm)
#driftVelReal = 0.00284     #(ArCo2 90/10 @ 350 V/cm)
#velCorr = driftVelReal / driftVelSim

# define cuts in Z
cuts = (0,10,20,30,40,50,60)

resUs = dict([(i, ROOT.TH1D("StatsResX"+str(cuts[i]), 
                            "Cosmic Residuals Z'", 500,-1,1)) for i in range(len(cuts))])
resVIDs = dict([(i, ROOT.TH1D("StatsResXID"+str(cuts[i]), 
                              "Cosmic Residuals X w/o clusters of size 1 edges", 
                              500,-1,1)) for i in range(len(cuts))])
resVs = dict([(i, ROOT.TH1D("StatsResV"+str(cuts[i]), 
                            "Cosmic Residuals V (X')", 500,-1,1)) for i in range(len(cuts))])

cl2Ds = dict([(i, ROOT.TH1D("cl2Ds"+str(cuts[i]), 
                            "Cluster Size Distributions as Function of Z", 50,0,50)) 
              for i in range(len(cuts))])

#resUVs = dict([(i, ROOT.TH1D("StatsResXY"+str(cuts[i]), 
#                             "Cosmic Residuals X'Y'", 500,-1,1)) for i in range(len(cuts))])
resVsUs = dict([(i, ROOT.TH2D("StatsResXsYs"+str(cuts[i]), 
                              "Cosmic Residuals X'Y'", 500,-0.5,0.5,
                              500,-0.5,0.5)) for i in range(len(cuts))])

clSize2D = ROOT.TH1D("clSize2D", "2D Cluster Size Distribution", 100,0,100)
clSize2D.SetFillColor(ROOT.kBlack)
clSize = ROOT.TH1D("clSize", "Cluster Size Distribution", 100,0,100)
clSize.SetLineColor(ROOT.kRed+3)

chi2prob = ROOT.TH1D("chi2prob", "ChiSqu Probability Distribution", 500,0,1)
chi2raw = ROOT.TH1D("chi2raw", "ChiSqu2 / NDF", 200,0,10)

diffT = ROOT.TF1("diffT","[0]*TMath::Sqrt(x+[1])",0,75)
diffT.SetNpx(1000)
diffT.SetParameter(0,0.0227*10000)
diffT.SetParameter(1,15.)

chi2func = ROOT.TF1("meh", "[0]*x*TMath::Exp(-2*x)",0,10)
chi2func.SetParameter(0,2500)


for f in range(25) :
    
    file = files[f]
    #file = "/nfs/hicran/data/tpc/fopi/2010/decoded/runC_1703.reco.root"
    print(file)
    Rfile = ROOT.TFile(file, "read")
    tree = Rfile.Get("cbmsim")
    tree.SetBranchStatus("*", 0)
   #tree.SetBranchStatus("PndTpcSLResiduals.*", 1)
    tree.SetBranchStatus("TrackFitStat.*", 1)
   #tree.SetBranchStatus("PndTpcCluster.*", 1)
   # tree.SetBranchStatus("TrackPostFit.*", 1)
    
    for e in tree :
        for tfs in e.TrackFitStat :
            chi2 = tfs.getChi2()
            #NDF = tfs.getNDF()
            NDIM=4
            
            numHits = tfs.GetHitPositionsZ().size()
            chi2Prob = ROOT.TMath.Prob(chi2, 2*numHits-NDIM)
            chi2prob.Fill(chi2Prob)
            chi2raw.Fill(chi2/(2*numHits-NDIM))

            mom = tfs.GetMom()
            mom.SetMag(1.)
            #build orthogonal vector to fix plane - res x mom
            vecX = ROOT.TVector3(1,0,0)
            vecZ = ROOT.TVector3(0,0,1)
            #define new coordinate system for this track
            u = mom.Cross(vecX)
            v = mom.Cross(u)
            
            for p in range(numHits) :
                z = tfs.GetHitPositionsZ().at(p) 
                x = tfs.GetHitPositionsX().at(p)
                y = tfs.GetHitPositionsY().at(p)
                
                xyRad = math.sqrt(x**2 + y**2)

                resX = tfs.GetResX().at(p)
                resY = tfs.GetResY().at(p)
                resZ = tfs.GetResZ().at(p)
                res = ROOT.TVector3(resX,resY,resZ)
                
                resU = res.Dot(u)
                resV = res.Dot(v)                
                
                cl2Dsize = tfs.Get2DClSizes().at(p)
                clTotSize = tfs.GetClSizes().at(p)
                clSize2D.Fill(cl2Dsize)
                clSize.Fill(clTotSize)
                
                for i in range(len(cuts)) :
                    if z < cuts[i] :
                        cl2Ds[i-1].Fill(cl2Dsize)
                        
                        resUs[i-1].Fill(resU)
                        resVs[i-1].Fill(resV)
#                        resUVs[i-1].Fill(math.sqrt(resX**2 + resY**2)) 
                        resVsUs[i-1].Fill(resV, resU)
                        
                        #compare with these filters:
                        if cl2Dsize > 1 :
                            resVIDs[i-1].Fill(resV)
                            
                            #if xyRad > 8 and xyRad < 12:
                            #if numHits > 20 :
                                                  
                        break
                    
                x = tfs.GetHitPositionsX().at(p)
                y = tfs.GetHitPositionsY().at(p)
                occXY.Fill(x,y)
                occZ.Fill(z)
                recoMom.Fill(tfs.GetP())
            
                
outfile.cd()

c1 = ROOT.TCanvas()
c1.Divide(3,2)
for i in range(6) :
    c1.cd(i+1)
    resVsUs[i].Draw("COLZ")
    resVsUs[i].Write()
c2 = ROOT.TCanvas()
occXY.Draw("COLZ")
occXY.Write()
c3 = ROOT.TCanvas()
occZ.Draw()
occZ.Write()

diffV = ROOT.TGraph(6)
diffV.SetName("diffX")
diffV.SetTitle("V(X') Resolution as function of Drift Length")
diffVID = ROOT.TGraph(6)
diffVID.SetMarkerColor(ROOT.kRed+2)
bckgrShare = ROOT.TGraph(6)
bckgrShare.SetName("bckgShare")
bckgrShare.SetTitle("Share of background (from fits)")

                            

c4 = ROOT.TCanvas()
c4.Divide(3,2)
for i in range(6) :
    c4.cd(i+1)
    resVs[i].SetFillColor(ROOT.kAzure-8)
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
    fit.SetParLimits(3, 0, testfit.GetParameter(0)/4.)
    fit.SetParameter(4, 0)
    fit.SetParLimits(4, testfit.GetParameter(1)-0.08, testfit.GetParameter(1)+0.08)
    fit.SetParameter(5, testfit.GetParameter(2))
    fit.SetParLimits(5, testfit.GetParameter(2), testfit.GetParameter(2)*30)
    resVs[i].Fit(fit, "+", "", -1,1)
    diffV.SetPoint(i,cuts[i]+5,fit.GetParameter(2)*10000)
    resVs[i].Write()
    #calculate ratio of central and background integrals
    ratio = fit.GetParameter(3)*fit.GetParameter(5)/(fit.GetParameter(0)*fit.GetParameter(2))
    bckgrShare.SetPoint(i,cuts[i]+5,ratio)
                             

c5 = ROOT.TCanvas()
c5.Divide(3,2)
for i in range(6) :
    c5.cd(i+1)
    ROOT.gPad.SetLogy(1)
    cl2Ds[i].Draw()


c6 = ROOT.TCanvas()
c6.Divide(3,2)
for i in range(6) :
    c6.cd(i+1)
    resUs[i].SetFillColor(ROOT.kAzure-8)
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
    resUs[i].Write()

c7 = ROOT.TCanvas()
c7.Divide(3,2)
for i in range(6) :
    c7.cd(i+1)
    
    resVIDs[i].SetFillColor(ROOT.kAzure-8)
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
    
    fit.SetParameter(3, 10)
    fit.SetParLimits(3, 0, testfit.GetParameter(0)/3.)
    fit.SetParameter(4, 0)
    fit.SetParLimits(4, testfit.GetParameter(1)-0.8, testfit.GetParameter(1)+0.8)
    fit.SetParameter(5, testfit.GetParameter(2)*2)
    fit.SetParLimits(5, testfit.GetParameter(2), testfit.GetParameter(2)*50)

    resVIDs[i].Fit(fit, "+", "", -1,1)
    diffVID.SetPoint(i,cuts[i]+5,fit.GetParameter(2)*10000)
    resVIDs[i].Write()

c8 = ROOT.TCanvas()
bckgrShare.SetMarkerStyle(21)
bckgrShare.GetYaxis().SetRangeUser(0,1)
bckgrShare.GetXaxis().SetTitle("Drift Length Z (cm)")
bckgrShare.Draw("ALP")
bckgrShare.Write()

c9 = ROOT.TCanvas()
diffV.SetMarkerStyle(21)
diffV.Draw("ALP")
diffV.GetXaxis().SetTitle("Drift Length Z (cm)")
diffV.GetYaxis().SetTitle("X Resolution (#mu m)")
diffV.GetYaxis().SetRangeUser(0,2000)
diffVID.SetMarkerStyle(21)
diffVID.Draw("LP")
diffV.Write()
diffVID.Write()
diffT.SetLineStyle(4)
diffT.Draw("same")


c10 = ROOT.TCanvas()
clSize2D.Draw()
clSize.Draw("same")

c11 = ROOT.TCanvas()
c11.cd()
ROOT.gPad.SetLogy(1)
chi2prob.Draw()

c12 = ROOT.TCanvas()
chi2raw.Draw()
chi2func.SetNpx(1000)
chi2func.Draw("same")

input()

outfile.Close()
