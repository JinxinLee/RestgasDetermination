import ROOT, glob
dir = "/nfs/hicran/data/tpc/fopi/2010/reconstructed"

ROOT.gROOT.ProcessLine(".x rootlogon.C") 
resX = ROOT.TH1D("StatsX", "Cosmic Residuals X", 500,-1,1)
resY = ROOT.TH1D("StatsY", "Cosmic Residuals Y", 500,-1,1)
failed = ROOT.TH1D("Failed", "Failed Hits", 30,0,30)

for file in glob.glob(dir + "/*.reco.root"):
    print(file)
    Rfile = ROOT.TFile(file, "read")
    tree = Rfile.Get("cbmsim")
    tree.SetBranchStatus("*", 0)
    tree.SetBranchStatus("PndTpcSLResiduals.*", 1)
    tree.SetBranchStatus("TrackFitStat.*", 1)
    
    for e in tree :
        for i in e.PndTpcSLResiduals :
            resX.Fill(i.X())
            resY.Fill(i.Y())
        for i in e.TrackFitStat :
            failed.Fill(i.getFailedHits())
            
            
c1 = ROOT.TCanvas()
resX.Draw()
c2 = ROOT.TCanvas()
resY.Draw()
c3 = ROOT.TCanvas()
failed.Draw()
input()

