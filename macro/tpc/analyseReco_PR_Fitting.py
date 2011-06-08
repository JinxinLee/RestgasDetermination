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

preliminary = 0
numFiles = 10000000
numEntries = 10000000

panda=1
mult = 1
minNumHits = 10

runs = "reco"
dir = "/nfs/nas/data/panda/tpc/SIM/momres/mult%i" %mult
outdir = "./"

#argument parsing:
for iarg in range(len(sys.argv)) :
    arg = sys.argv[iarg]
    if arg == "--prelim" :
        preliminary = 1
    if arg == "-path" :
        dir = sys.argv[iarg+1];
    if arg == "-outpath" :
        outdir = sys.argv[iarg+1];        
    if arg == "-n" :
        numFiles = int(sys.argv[iarg+1])
    if arg == "-nEntries" :
        numEntries = int(sys.argv[iarg+1])
    if arg == "-runs" :
        runs = sys.argv[iarg+1];    
    if arg == "-TB" :
        panda = 0
    if arg == "-minnumhits" :
        minNumHits = int(sys.argv[iarg+1]);
    if arg == "-mult" :
        mult = int(sys.argv[iarg+1]);
#finished argument parsing

runList=[]
dashIndex = runs.find("-")
if dashIndex > 0 :
    runs = runs.split("-")
    runList = range(int(runs[0]), int(runs[1])+1)
if dashIndex < 0 and len(runs) > 1 :
    runList = [runs]




ROOT.gROOT.ProcessLine(".x rootlogon.C") 
ROOT.gROOT.ProcessLine('gSystem->Load("libPhysics")')
ROOT.gROOT.ProcessLine('gStyle->SetPalette(1)')
ROOT.gROOT.ProcessLine('gROOT->SetStyle("Plain")')

files = glob.glob(dir + "/*.reco.root")
files.sort()

print files
print "minnumhits =  %i" %minNumHits
print "multiplicity =  %i" %mult
print "numEntries =  %i" %numEntries



#  ---------------------------------------- ANA LOOP --------------------------------------

fcounter = 0

simpath=""
for env in os.environ.keys() :
    if env == "PANDAPATH" and len(os.environ[env]) > 0 :
        simpath = os.environ[env]
        print "Setting pandaroot path to %s" %simpath

poolfile=""
planefile=""

if panda :
    poolfile = simpath + "/tpc/Hexagons0.15.dat"
    planefile = simpath + "/tpc/pndhexplane0.15.dat"
    print "using panda padplane"
else :
    poolfile = simpath + "/tpc/TestBench/PadShapePrototype.dat"
    planefile = simpath + "/tpc/TestBench/padPlane_FOPI.dat"
    print "using FOPI padplane"

pool = ROOT.PndTpcPadShapePool(poolfile)
plane = ROOT.PndTpcPadPlane(planefile, pool)


print "loop over files"
for file in files :
    #check if it appears in the runList
    if fcounter > numFiles :
        print "reached last file"
        continue
        
    if len(runList) >= 1 :
        found = 0
        for i in runList :
            if file.find(str(i)) >= 0 :
                found = 1
        if found == 0 :
            print "file not found"
            continue
        
        
    mcfile = file[0:file.find(".reco.root")]
    mcfile += ".mc.root"
    
    
    print "Processing files: "
    print(file)
    print(mcfile)
    
    #~ print "Parsing filename: "    
    #~ pdg = int(file[file.find("pdg_")+4:file.find("__mult_")])
    #~ mult = int(file[file.find("__mult_")+7:file.find("__mom_")])
    #~ mom = float(file[file.find("__mom_")+6:file.find("__theta_")])
    #~ theta = float(file[file.find("__theta_")+8:file.find(".reco")]) 
    
    print "Parsing filename: "    
    pdg = int(file[file.find("PDG")+3:file.find("_mom")])
    mom = float(file[file.find("_mom")+4:file.find("_deg")])
    theta = float(file[file.find("_deg")+4:file.find("_mult")])
    
    print "pdg = %i" %pdg
    print "multiplicity = %i" %mult
    print "momentum = %f" %mom
    print "theta = %f" %theta

    
    fcounter+=1
    
    MCfile = ROOT.TFile.Open(mcfile, "read")
    mctree = MCfile.Get("cbmsim")
    mctree.SetBranchStatus("*", 0)
    mctree.SetBranchStatus("MCTrack.*", 1)

    Rfile = ROOT.TFile.Open(file, "read")
    tree = Rfile.Get("cbmsim")
    tree.SetBranchStatus("*", 0)
    tree.SetBranchStatus("TrackPostFit.*", 1)
    tree.SetBranchStatus("TrackFitStat_0.*", 1)
    #tree.SetBranchStatus("TrackFitStat_1.*", 1)
    tree.SetBranchStatus("RiemannTrack.*", 1)
    tree.SetBranchStatus("PndTpcCluster.*", 1)
    
    
    recoMom0 = ROOT.TH1D("recoMom0", "Rec. Momenta RK", 500,0,10)
    recoMom1 = ROOT.TH1D("recoMom1", "Rec. Momenta Geane", 500,0,10)

    recoEff = ROOT.TH1D("recoEff", "unique rec. Tracks with > minNumHits", mult+1, 0,mult+1) 
    splitting = ROOT.TH1D("splitting", "split Tracks with > minNumHits", 8*mult+1, 0,8*mult+1) 
    trkPurity = ROOT.TH1D("trkPurity", "Track Purity of Tracks with > minNumHits", 101,0,1.01) 
    trkCompleteness = ROOT.TH1D("trkCompleteness", "Track Completeness of Tracks with > minNumHits", 101,0,1.01) 

    effVsTheta = ROOT.TH2D("effVsTheta", "Reco Efficiency vs theta", 37,0,185, 101,0,1.01) 
    splitVsTheta = ROOT.TH2D("splitVsTheta", "Track Splitting vs theta", 37,0,185, 101,0,1.01) 
    purityVsTheta = ROOT.TH2D("purityVsTheta", "Track Purity vs theta", 37,0,185, 101,0,1.01) 
    complVsTheta = ROOT.TH2D("complVsTheta", "Track Completeness vs theta", 37,0,185, 101,0,1.01) 
    
    foundVsTheta = ROOT.TH2D("foundVsTheta", "percentage of found tracks (>50% of clusters) vs theta", 37,0,185, 101,0,1.01) 

    effVsThetaA = ROOT.TH2D("effVsThetaA", "Reco Efficiency vs theta", 37,0,185, mult+1, 0,mult+1) 
    splitVsThetaA = ROOT.TH2D("splitVsThetaA", "Track Splitting vs theta", 37,0,185, 8*mult+1, 0,8*mult+1) 
    purityVsThetaA = ROOT.TH2D("purityVsThetaA", "Track Purity vs theta", 37,0,185, 101,0,1.01) 
    complVsThetaA = ROOT.TH2D("complVsThetaA", "Track Completeness vs theta", 37,0,185, 101,0,1.01)     
    
    outname = outdir
    outname += file[file.rfind("/"):file.find(".reco")]
    #outname+= str(mult)
    outname += "_minnumhits"
    outname+= str(minNumHits)
    outname += ".ana.root"
    print outname
    outfile = ROOT.TFile(outname, "recreate")

    
    counter = 0
    nGlobTrks = 0
    nUniqueTrks = 0
    index = 0
    meanEff = 0
    meanSplit = 0
    meanPurity = 0
    meanCompl = 0
    
    foundTracks = 0


    for e in tree :
        nTracks = e.TrackPostFit.GetEntriesFast()  
        nCluster = e.PndTpcCluster.GetEntriesFast()
        
        # find out how many MCIDs there are and how many clusters belong to each MCID
        globalCol = ROOT.McIdCollection()
        nClusterPerID={0:0}
        for cl in e.PndTpcCluster :
            globalCol.AddIDCollection(cl.mcId());
            cdDomId=int(cl.mcId().DominantID().mctrackID())
            #print cdDomId
            if (cdDomId in nClusterPerID) :
                nClusterPerID[cdDomId] += 1
            else :
                nClusterPerID[cdDomId] = 1
            
        #print nClusterPerID

        nMCIDs = globalCol.nIDs()
        
        mctree.GetEntry(index)
        nMCTracks = mctree.MCTrack.GetEntriesFast()
        index+=1
        
        if nMCIDs != mult :
            print "nMCIDs != mult, skipping event %i" %index
            continue
        if nMCTracks != mult :
            print "nMCTracks != mult, skipping event %i" %index
            continue        
        if nMCIDs == 0 :
            print "no McIds, skipping event %i" %index
            continue
            
        #print "Found %i MCIDs in event" %nMCIDs
            
        nRecoTrks = 0
        nSplitTrks = 0
        nTrks = 0
        DominantIDs = []
        
        
        maxClustersPerID={0:0} # for finding the track with the most hits
        
        
        for rtrk in e.RiemannTrack :
            TrkCol = rtrk.mcid()
            ntrkMCIDs = TrkCol.nIDs()
            #print "Found %i MCIDs in track" %ntrkMCIDs
             
            
            numHits = rtrk.getNumHits()
            #print "Found %i hits in track" %numHits
            DominantID=rtrk.mcid().DominantID().mctrackID()
            
            

            if numHits >= minNumHits :
                nTrks += 1
                nGlobTrks += 1
                purity = rtrk.mcid().MaxRelWeight()
                trkPurity.Fill(purity)
                purityVsThetaA.Fill(theta, purity) 
                meanPurity += purity
                
                compleetness = numHits/float(nClusterPerID[DominantID])
                
                if (DominantID in maxClustersPerID) :
                    if maxClustersPerID[DominantID] < compleetness :
                       maxClustersPerID[DominantID] = compleetness
                else :
                    maxClustersPerID[DominantID] = compleetness 
                
                
                DominantIDs.append(DominantID)
                
                if DominantIDs.count(DominantID) == 1 :
                    nRecoTrks+=1
                else :
                    nSplitTrks+=1
                    
                #print DominantIDs
                
        # end loop over tracks        
                
        for id, compleetness in maxClustersPerID.iteritems() :      
            trkCompleteness.Fill(compleetness)
            complVsThetaA.Fill(theta, compleetness) 
            meanCompl += compleetness
            nUniqueTrks += 1
            if compleetness > 0.5 :
                foundTracks += 1;
                
            
        recoEff.Fill(nRecoTrks/float(mult))
        effVsThetaA.Fill(theta, nRecoTrks/float(mult))
        meanEff += nRecoTrks/float(mult)
        
        splitting.Fill(nSplitTrks)
        splitVsThetaA.Fill(theta, nSplitTrks)  
        meanSplit += nSplitTrks
        
        

        for tfs in e.TrackFitStat_0 :
            if tfs.GetHitIDs().size()>= minNumHits :
                recoMom0.Fill(tfs.GetP())
        #for tfs in e.TrackFitStat_1 :
             #if tfs.GetHitIDs().size()>= minNumHits :
        #       recoMom1.Fill(tfs.GetP())
            
            
        counter+=1   
        
        if counter > numEntries :
            break
            
    # end loop over tree

    print counter
    
    
    meanEff /= float(counter)
    effVsTheta.Fill(theta, meanEff)    
    
    meanSplit /= float(counter)
    splitVsTheta.Fill(theta, meanSplit)  
      
    meanCompl /= float(nUniqueTrks)
    complVsTheta.Fill(theta, meanCompl) 
          
    meanPurity /= float(nGlobTrks)
    purityVsTheta.Fill(theta, meanPurity) 
    
    foundVsTheta.Fill(theta, foundTracks/float(counter*mult))

    outfile.cd()

    c1 = ROOT.TCanvas()
    #title =  "pdg = %i, multiplicity = %i, momentum = %f, theta = %f" %pdg %mult %mom %theta
    c1.SetTitle(dir)
    c1.Divide(5,3)

    c1.cd(1)
    recoEff.Draw()
    recoEff.Write()
    c1.cd(2)
    splitting.Draw()
    splitting.Write()
    c1.cd(3)
    trkPurity.Draw()
    trkPurity.Write()
    c1.cd(4)
    trkCompleteness.Draw()
    trkCompleteness.Write()
    c1.cd(5)
    recoMom0.Draw()
    recoMom0.Write()
    c1.cd(6)
    effVsTheta.Draw("colz")
    effVsTheta.Write()
    c1.cd(7)
    splitVsTheta.Draw("colz")
    splitVsTheta.Write()
    c1.cd(8)
    purityVsTheta.Draw("colz")
    purityVsTheta.Write()
    c1.cd(9)
    complVsTheta.Draw("colz")
    complVsTheta.Write()
    c1.cd(10)
    recoMom1.Draw()
    recoMom1.Write()
    c1.cd(11)
    effVsThetaA.Draw("colz")
    effVsThetaA.Write()
    c1.cd(12)
    splitVsThetaA.Draw("colz")
    splitVsThetaA.Write()
    c1.cd(13)
    purityVsThetaA.Draw("colz")
    purityVsThetaA.Write()
    c1.cd(14)
    complVsThetaA.Draw("colz")
    complVsThetaA.Write()    
    c1.cd(15)
    foundVsTheta.Draw("colz")
    foundVsTheta.Write()    

    #input()

    c1.Write()
    outfile.Close()
    
#  ------------------------------- END OF ANA LOOP ---------------------------------------           

    




