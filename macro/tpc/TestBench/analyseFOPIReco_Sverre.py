
import ROOT, glob, math
from ROOT import std
#dir = "/nfs/hicran/data/tpc/fopi/2010/productions/reconstructed3"
dir = "/nfs/hicran/data/tpc/fopi/2010/decoded"

outfile = ROOT.TFile("anaOutSverre.root", "recreate")

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
occZR = ROOT.TH2D("OccZ", "Cluster Z occupancy created from cosmic tracks",
                 200,0,75,200,0,20)
rmsVvsPhiTheta = ROOT.TH2D("RMSVvsPhiTheta", "RMS V",
                           600,-3.5,3.5,600,-3.5,3.5)
rmsUvsPhiTheta = ROOT.TH2D("RMSUvsPhiTheta", "RMS U ",
                           600,-3.5,3.5,600,-3.5,3.5)
occPhiTheta = ROOT.TH2D("occPhiTheta", "Cluster track occupancy Phi/Theta",
                           600,-3.5,3.5,600,-3.5,3.5)
files = glob.glob(dir + "/*.reco.root")
files.sort()


# -------------------- driftVel correction --------------------------------
#driftVelSim = 0.00055192   #(ArCo2 70/30 @ 250 V/cm)
#driftVelReal = 0.00284     #(ArCo2 90/10 @ 350 V/cm)
#velCorr = driftVelReal / driftVelSim

# define cuts in Z
cuts = (0,10,20,30,40,50,60)

#has to have same binwidths and ranges as occXYs
resVvsXYs = dict([(i, ROOT.TH2D("StatsResVvsXY"+str(cuts[i]), 
                                "Cosmic Residuals V", 200,-15,15,
                                200,-15,15)) for i in range(len(cuts))])
resUvsXYs = dict([(i, ROOT.TH2D("StatsResUvsXY"+str(cuts[i]), 
                                "Cosmic Residuals U", 200,-15,15,
                                200,-15,15)) for i in range(len(cuts))])
occXYs = dict([(i,ROOT.TH2D("OccXYs"+str(cuts[i]), 
                            "Cluster XY occupancy created from cosmic tracks",200,-15,15,
                            200,-15,15)) for i in range(len(cuts))])
resVvsXY =ROOT.TH2D("StatsResVvsXY", 
                    "Cosmic Residuals V", 200,-15,15,
                    200,-15,15)
resUvsXY =ROOT.TH2D("StatsResUvsXY", 
                    "Cosmic Residuals V", 200,-15,15,
                    200,-15,15)

#for f in range(10) :
for file in files :    
    #list = file.split("_")
    #num = int(list[1][2:4])
 #   file = files[f]
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
            mom = tfs.GetMom()
            mom.SetMag(1.)
            #build orthogonal vector to fix plane - res x mom
            vecX = ROOT.TVector3(1,0,0)
            vecZ = ROOT.TVector3(0,0,1)
            #define new coordinate system for this track
            u = mom.Cross(vecX)
            v = mom.Cross(u)   
            phi =(mom.Phi())
            theta=(mom.Theta())
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
                rmsVvsPhiTheta.Fill(phi,theta,resV*resV)
                rmsUvsPhiTheta.Fill(phi,theta,resV*resV)
                resVvsXY.Fill(x,y,resV*resV) 
                resUvsXY.Fill(x,y,resU*resU)
                occPhiTheta.Fill(phi,theta)
                for i in range(len(cuts)) :
                    if z < cuts[i] :
                        resVvsXYs[i].Fill(x,y,resV*resV) 
                        resUvsXYs[i].Fill(x,y,resU*resU)
                        occXYs[i].Fill(x,y)
                        break
                            #compare with these filters:
                   
                x = tfs.GetHitPositionsX().at(p)
                y = tfs.GetHitPositionsY().at(p)
                occXY.Fill(x,y)
                occZ.Fill(z)
                occZR.Fill(z,xyRad,1/xyRad);
                recoMom.Fill(tfs.GetP())
            
                
outfile.cd()


c2 = ROOT.TCanvas("occXYcan","occXY")
occXY.GetXaxis().SetTitle("cm")
occXY.GetYaxis().SetTitle("cm")
occXY.Draw("COLZ")
occXY.Write()
c3 = ROOT.TCanvas("occZcan", "occZ")
occZ.GetXaxis().SetTitle("cm")
occZ.Draw()
occZ.Write()
c4 = ROOT.TCanvas("occZRcan", "occZR")
occZR.GetXaxis().SetTitle("cm")
occZR.GetYaxis().SetTitle("cm")
occZR.Draw("colz")
occZR.Write()
c13 = ROOT.TCanvas("rmsVvsXYcan","rmsVvsXYcan")
c13.Divide(3,2)
c14 = ROOT.TCanvas("rmsUvsXYcan","rmsUvsXYcan")
c14.Divide(3,2)
c15 = ROOT.TCanvas("occXYcans","occXYs")
c15.Divide(3,2)
binx=occXY.GetNbinsX()
biny=occXY.GetNbinsY()

for i in range(6):

    c13.cd(i+1).SetLogz()
    resVvsXYs[i].SetMaximum(0.8)
#    resVvsXYs[i].Smooth(1)
    resVvsXYs[i].Draw("colz")
    c14.cd(i+1).SetLogz()
    resUvsXYs[i].SetMaximum(0.8)
#    resUvsXYs[i].Smooth(1)
    resUvsXYs[i].Draw("colz")
    c15.cd(i+1)
    occXYs[i].Draw("colz")
    resVvsXYs[i].Write()
    resUvsXYs[i].Write()
c20= ROOT.TCanvas("rmsVvsXYcan","rmsVvsXY")
c20.SetLogz()
resVvsXY.SetMaximum(0.8)
resVvsXY.Draw("colz")
resVvsXY.Write()
c21= ROOT.TCanvas("rmsUvsXYcan","rmsUvsXY")
c21.SetLogz()
resUvsXY.SetMaximum(0.8)
resUvsXY.Draw("colz")
resUvsXY.Write()
c16 = ROOT.TCanvas("occPhiThetaCan","occPhiTheta")
occPhiTheta.GetXaxis().SetTitle("phi")
occPhiTheta.GetYaxis().SetTitle("theta")
occPhiTheta.Draw("COLZ")
occPhiTheta.Write()

c17 = ROOT.TCanvas("rmsVvsPhiThetaCan","rmsVvsPhiTheta")
rmsUvsPhiTheta.GetXaxis().SetTitle("phi")
rmsUvsPhiTheta.GetYaxis().SetTitle("theta")


rmsVvsPhiTheta.GetXaxis().SetTitle("phi")
rmsVvsPhiTheta.GetYaxis().SetTitle("theta")

binx=occPhiTheta.GetNbinsX()
biny=occPhiTheta.GetNbinsY()
    
for u in range(binx):
    for v in range(biny):
        rms=math.sqrt(rmsUvsPhiTheta.GetBinContent(u,v))
        if rms > 0:
            rmsUvsPhiTheta.SetBinContent(u,v,rms/math.sqrt(occPhiTheta.GetBinContent(u,v)))
            
        rms=math.sqrt(rmsVvsPhiTheta.GetBinContent(u,v))
        if rms > 0:
            rmsVvsPhiTheta.SetBinContent(u,v,rms/math.sqrt(occPhiTheta.GetBinContent(u,v)))


rmsVvsPhiTheta.Draw("COLZ")
rmsVvsPhiTheta.Write()
c18 = ROOT.TCanvas("rmsUvsPhiThetaCan","rmsUvsPhiTheta")
rmsUvsPhiTheta.Draw("COLZ")
rmsUvsPhiTheta.Write()
input()

outfile.Close()
