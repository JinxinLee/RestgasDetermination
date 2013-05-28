#include <iostream>
#include <sstream>

#include<TApplication.h>
#include<TCanvas.h>
#include<TROOT.h>
#include<TString.h>
#include<TChain.h>
#include<TFile.h>
#include<TClonesArray.h>
#include<TSystem.h>
#include<TH1.h>
#include<TH2.h>
#include<TF1.h>
#include<TRotation.h>
#include<TVector3.h>
#include<TMath.h>
#include<TGaxis.h>
#include<TNtuple.h>
#include<TMatrixD.h>
#include<TStopwatch.h>
#include<TNtuple.h>
#include<PndMCTrack.h>
#include<PndTrack.h>
#include<PndSdsMCPoint.h>
#include "PndLmdDim.h"

//lmd track
#include<PndLinTrack.h>
#include<PndTrackCand.h>
#include<PndTrackCandHit.h>
#include<PndSdsHit.h>
#include<PndSdsClusterStrip.h>
#include<PndSdsDigiStrip.h>
#include<PndSdsDigiPixel.h>
#include<PndSdsClusterPixel.h>
#include<PndSdsMergedHit.h>

// needed for geane backtracking
#include<FairRunAna.h>
#include<FairRootManager.h>
#include<FairGeane.h>
#include<FairRtdbRun.h>
#include<FairRuntimeDb.h>
#include<FairParRootFileIo.h>
#include<FairLogger.h>
#include<FairTrackParH.h>

#include<string>

using namespace std;
int main(int __argc,char *__argv[]) {
  //gROOT->Macro("/PANDA/pandaroot/macro/lmd/Style_Imported_Style.C");
  // gROOT->SetStyle("Imported_Style"); 

  //TODO: read this like params!
  //  const int nEvents=500000;
  int nEvents=1000;
  int nMCtracks=1;
  int startEvent=0;
  TString storePath="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutput";
  double Plab=15.;
  int verboseLevel=5;
  int mh = 0;//if>0 : use merged hit collection
  std::string startStr="", momStr="", nStr="", pathStr="", verbStr="" , mcTrkStr="", usedMHStr="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    
    std::cout << "This is script for comparision reconstructed and simulated tracks with parameters\n"
	      <<"-s start event \n"
	      <<"-n Number of events \n"
	      <<"-t Number of trks per event \n"
	      <<"-mom Beam Momentum \n"
	      <<"-path path to the file(s) \n"
	      <<"-v verbose Level (if>0, print out some information) \n"
	      <<"-mh if>0: use merged hits\n"
	      <<"Have fun! \n"
	      << std::endl;
    return 0;
  } 
  while ((optind < (__argc-1) ) && (__argv[optind][0]=='-')) {
    bool found=false;
    std::string sw = __argv[optind];
    if (sw=="-s") {
      optind++;
      startStr = __argv[optind];
      found=true;
    }
    if (sw=="-n"){
      optind++;
      nStr = __argv[optind];
      found=true;
    }
    if (sw=="-t"){
      optind++;
      mcTrkStr = __argv[optind];
      found=true;
    }
    if (sw=="-path"){
      optind++;
      pathStr = __argv[optind];
      found=true;
    }
    if (sw=="-mom"){
      optind++;
      momStr = __argv[optind];
      found=true;
    }
    if (sw=="-v"){
      optind++;
      verbStr = __argv[optind];
      found=true;
    }
   
    if (sw=="-mh"){
      optind++;
      usedMHStr = __argv[optind];
      found=true;
    }
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
  }

  std::stringstream startSStr(startStr), momSStr(momStr), nSStr(nStr), pathSStr(pathStr), verbSStr(verbStr), mcTrkSStr(mcTrkStr), usedMHSStr(usedMHStr);

  startSStr >> startEvent;
  momSStr >> Plab;
  nSStr >> nEvents;
  pathSStr >> storePath;
  verbSStr >> verboseLevel;
  mcTrkSStr >> nMCtracks;
  usedMHSStr >> mh;
  cout<<"====================================="<<endl;
  cout<<"some INFO about this macro params: "<<endl;
  cout<<"expected number of tracks per event: "<<nMCtracks<<endl;
  cout<<"expected number of events: "<<nEvents<<endl;
  cout<<"Pbeam: "<<Plab<<endl;
  cout<<"Will be used Path: "<<storePath<<endl;
  cout<<"====================================="<<endl;
  //void MCandRECTrkmatches(const int nEvents=2, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=3, double dv=0.5, bool no4d=false)
  //{
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  // gROOT->LoadMacro("line3Dfit.C");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // ---- Input files --------------------------------------------------------
  TString simMC=storePath+"/Lumi_MC_";
  simMC += startEvent;
  simMC += ".root";
  TChain tMC("cbmsim");
  tMC.Add(simMC);
  
  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  TChain tdigiHits("cbmsim");
  tdigiHits.Add(DigiFile);
  
  TString recHit=storePath+"/Lumi_reco";
  // if(dnu>0) recHit+="Merged";
  recHit +="_";
  recHit += startEvent;
  recHit += ".root";
  TChain tHits("cbmsim");
  tHits.Add(recHit);
  
  TString recHitmerged=storePath+"/Lumi_recoMerged_";
  recHitmerged += startEvent;
  recHitmerged += ".root";
  TChain tHitsMerged("cbmsim");
  if(mh)
    tHitsMerged.Add(recHitmerged);

  
  TString trkCand = storePath+"/Lumi_TCand_";
  trkCand += startEvent;
  trkCand += ".root";
  TChain tTrkCand("cbmsim");
  tTrkCand.Add(trkCand);

  TString recTrack;
  TChain tTrkRec("cbmsim");
  recTrack = storePath+"/Lumi_Track_";
  recTrack += startEvent;
  recTrack += ".root";
  tTrkRec.Add(recTrack);
  // if(kf<1){
  //   recTrack = storePath+"/Lumi_Track_";
  //   recTrack += startEvent;
  //   recTrack += ".root";
  //   tTrkRec.Add(recTrack);
  // }
  
  TString geaneFile = storePath+"/Lumi_Geane_";
  geaneFile += startEvent;
  geaneFile += ".root";
  TChain tgeane("cbmsim");
  tgeane.Add(geaneFile);
  
  // ---------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs";
  out += startEvent;
  out += ".root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------
  
  //--- MC info -----------------------------------------------------------------
  TClonesArray* true_tracks=new TClonesArray("PndMCTrack");
  tMC.SetBranchAddress("MCTrack",&true_tracks);  //True Track to compare
  
  TClonesArray* true_points=new TClonesArray("PndSdsMCPoint");
  tMC.SetBranchAddress("LMDPoint",&true_points);  //True Points to compare
  //----------------------------------------------------------------------------------
  
  
  //--- Digitization info ------------------------------------------------------------
  TClonesArray* fStripClusterArray;
  fStripClusterArray = new TClonesArray("PndSdsClusterPixel");
  tHits.SetBranchAddress("LMDPixelClusterCand",&fStripClusterArray);
  
  
  TClonesArray* fStripDigiArray;
    fStripDigiArray = new TClonesArray("PndSdsDigiPixel");
    tdigiHits.SetBranchAddress("LMDPixelDigis",&fStripDigiArray);

  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array;
  if(mh>0){  
    rechit_array = new TClonesArray("PndSdsMergedHit");
    tHitsMerged.SetBranchAddress("LMDHitsMerged",&rechit_array);  //Points for Tracks
    if(verboseLevel>0)  cout<<"LMDHitsMerged will be used as hits!!"<<endl;
  }
  else{
    rechit_array = new TClonesArray("PndSdsHit");
    tHits.SetBranchAddress("LMDHitsPixel",&rechit_array);  //Points for Tracks
  }
  
  //----------------------------------------------------------------------------------
  
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndTrack");
  tTrkRec.SetBranchAddress("LMDPndTrack",&rec_trk);  //Tracks

  cout<<"Here we go to GEANE file: "<<endl;
  //----------------------------------------------------------------------------------
  
  //--- Geane info ------------------------------------------------------------------
  // TClonesArray* geaneArray =new TClonesArray("FairTrackParP");
  TClonesArray* geaneArray =new TClonesArray("FairTrackParH");
  tgeane.SetBranchAddress("GeaneTrackFinal",&geaneArray);  //Tracks with parabolic parametrisation
  
  cout<<"And we'll make some hists"<<endl;
  //--- Output histogram -----------------------------------------------------
  TH1 *hchi2 = new TH1F("hchi2","#chi^2 for reconstructed tracks;#chi^2;",1.5e2,0,15.);
  TH2 *hnRecnMC = new TH2F("hnRecnMC","Number reconstracted tracks vs. Number simulated tracks; N_{MC}; N_{rec}",
			   100,0,100,100,0,100);
  TH1 *hDiffIDs = new TH1I("hDiffIDs","Number of track-candidates with hits from diff. MC-track;N_{IDs}",10,0,10);
  TH1 *hntrkcand = new TH1F("hntrkcand","Number of track-candidates per event;N_{trk-cand}",100,0,100);
  TH1 *hntrk = new TH1F("hntrk","Number of tracks per event;N_{trk}",100,0,100);
  TH1 *hntrkmissed_I = new TH1F("hntrkmissed_I","Number of missed tracks per event (hit losses);N_{trk}",30,0,30);
  // TH1 *hntrkgood_I = new TH1F("hntrkgood_I","Number of good tracks per event (#phi, #theta trks);N_{trk}",30,0,30);
  TH1 *hntrkghost_I = new TH1F("hntrkghost_I","Number of ghost tracks per event (hits mixture);N_{trk}",30,0,30);
  TH1 *hntrkmissed_II = new TH1F("hntrkmissed_II","Number of missed tracks per event (trk search losses);N_{trk}",30,0,30);
  TH1 *hntrkgood_II = new TH1F("hntrkgood_II","Number of good tracks per event (hits matching);N_{trk}",30,0,30);
  TH1 *hntrkghost_II = new TH1F("hntrkghost_II","Number of ghost tracks per event  (repeated trk);N_{trk}",30,0,30);
  TH2 *hntrkmissedPhiTheta = new TH2F("hntrkmissedPhiTheta",";#delta#theta/#sigma#theta;#delta#phi/#sigma#phi",1000,0,100,1000,0,100);
  TH2 *hntrkcandvsIDs = new TH2F("hntrkcandvsIDs","Number of track-candidates per event vs Number of track-candidates with hits from diff. MC-track;N_{IDs};N_{trk-cand}",10,0,10,100,0,100);
  TH2 *hntrkcandvsMC = new TH2F("hntrkcandvsMC","Number of per event vs. Number of simulated tracks; N_{MC};N_{trk-cand}",100,0,100,100,0,100);

  TH2 *hnhitsvsIDs = new TH2F("hnhitsvsIDs","Number of hits per track-candidate vs Number of track-candidates with hits from diff. MC-track;N_{IDs};Number of hits",10,0,10,10,2,12);
 
  TH1 * hallTheta = new TH1F("hallTheta","#theta for all MC-track;#theta, rad",50,2e-3,10e-3);
  TH1 * hgoodTheta = new TH1F("hgoodTheta","#theta for good track-candidates",50,2e-3,10e-3);
  TH1 * hallPhi = new TH1F("hallPhi","#phi for all MC-track;#phi, rad",50,-355./113,355./113);
  TH1 * hgoodPhi = new TH1F("hgoodPhi","#phi for good track-candidates",50,-355./113,355./113);
  TH2 * hgoodPhiTheta = new TH2F("hgoodPhiTheta","#phi and #thete for good track-candidates",50,2e-3,10e-3,100,-355./113,355./113);
  TH2 * hallPhiTheta = new TH2F("hallPhiTheta","#phi & #theta for all MC",50,2e-3,10e-3,100,-355./113,355./113);

  TNtuple *ntuprecTrk = new TNtuple("ntuprecTrk","Info about reconstructed trks: goodTrk [0=good,+1=ghost from hit mixture, +2=similar as other trk]","x:y:z:mom:theta:phi:goodTrk");
  TNtuple *ntupMCTrk = new TNtuple("ntupMCTrk","Info about simulated trks: goodTrk [0=good, -1=missed in trk search, -2=little amount hits]","x:y:z:mom:theta:phi:goodTrk:nMChits:nREChits:nDoubleMChits");

  TH1 *hResMom = new TH1F("hResMom","P_{MC}-P_{rec};#deltaP,GeV/c",1e3,-1e-4,1e-4);
  TH1 *hErrMom = new TH1F("hErrMom","#sigma_{P};#sigmaP,GeV/c",1e3,0,1e-3);
  TH1 *hPullMom = new TH1F("hPullMom","(P_{MC}-P_{rec})/#sigma_{P};",1e3,-1e1,1e1);
  TH1 *hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);//TEST
  TH1 *hErrTheta = new TH1F("hErrTheta","#sigma(#theta_{rec});#sigma,rad",1e3,0,0.01);
  TH1 *hPullTheta = new TH1F("hPullTheta","(#theta_{MC}-#theta_{rec})/#sigma_{#theta};",1e2,-10,10);
  TH1 *hResPhi = new TH1F("hResPhi","#phi_{MC}-#phi_{rec};#delta#phi,rad",2e3,-1.,1.);
  TH1 *hErrPhi = new TH1F("hErrPhi","#sigma(#phi_{rec});#sigma,rad",1e3,0,0.1);
  TH1 *hPullPhi = new TH1F("hPullPhi","(#phi_{MC}-#phi_{rec})/#sigma_{#phi};",1e2,-10,10);
  TH1 *hResPointX = new TH1F("hResPointX","X_{MC}-X_{rec};#deltaX,cm",1e2,-2.,2.);
  TH1 *hResPointY = new TH1F("hResPointY","Y_{MC}-Y_{rec};#deltaY,cm",1e2,-2.,2.);
  TH1 *hResPointZ = new TH1F("hResPointZ","Z_{MC}-Z_{rec};#deltaZ,cm",1e3,-0.15,0.15);
  TH1 *hErrPointX = new TH1F("hErrPointX","#sigma_{X};#sigmaX,cm",1e3,0,1.);
  TH1 *hErrPointY = new TH1F("hErrPointY","#sigma_{Y};#sigmaY,cm",1e3,0,1.);
  TH1 *hErrPointZ = new TH1F("hErrPointZ","#sigma_{Z};#sigmaZ,cm",1e2,0,0.01);
  TH1 *hPullPointX = new TH1F("hPullPointX","(X_{MC}-X_{rec})/#sigma_{X};(X_{MC}-X_{rec})/#sigma_{X}",1e2,-10,10);
  TH1 *hPullPointY = new TH1F("hPullPointY","(Y_{MC}-Y_{rec})/#sigma_{Y};(Y_{MC}-Y_{rec})/#sigma_{Y}",1e2,-10,10);
  TH1 *hPullPointZ = new TH1F("hPullPointZ","(Z_{MC}-Z_{rec})/#sigma_{Z};(Z_{MC}-Z_{rec})/#sigma_{Z}",1e3,-100,100);
  TH1 *hResPointPx = new TH1F("hResPointPx","Px_{MC}-Px_{rec};#deltaPx, GeV/c",1e2,-0.01,0.01);
  TH1 *hErrPointPx = new TH1F("hErrPointPx","#sigma_{Px};#sigmaPx, GeV/c",1e3,0,0.01);
  TH1 *hPullPointPx = new TH1F("hPullPointPx","(Px_{MC}-Px_{rec})/#sigma_{Px};(Px_{MC}-Px_{rec})/#sigma_{Px}",1e2,-10,10);
  TH1 *hResPointPy = new TH1F("hResPointPy","Py_{MC}-Py_{rec};#deltaPy, GeV/c",1e2,-0.01,0.01);
  TH1 *hErrPointPy = new TH1F("hErrPointPy","#sigma_{Py};#sigmaPy, GeV/c",1e3,0,0.01);
  TH1 *hPullPointPy = new TH1F("hPullPointPy","(Py_{MC}-Py_{rec})/#sigma_{Py};(Py_{MC}-Py_{rec})/#sigma_{Py}",1e2,-10,10);
  TH1 *hResPointPz = new TH1F("hResPointPz","Pz_{MC}-Pz_{rec};#deltaPz, GeV/c",1e2,-1e-3,1e-3);
  TH1 *hErrPointPz = new TH1F("hErrPointPz","#sigma_{Pz};#sigmaPz, GeV/c",1e3,0,1e-1);
  TH1 *hPullPointPz = new TH1F("hPullPointPz","(Pz_{MC}-Pz_{rec})/#sigma_{Pz};(Pz_{MC}-Pz_{rec})/#sigma_{Pz}",1e2,-10,10);

  TH2 *hMCThetaResTheta = new TH2F("hMCThetaResTheta",";#theta_{MC}, rad;#Delta#theta, rad",1e2,0,0.012,1e3,-1e-2,1e-2);
  TH2 *hMCPhiResPhi = new TH2F("hMCPhiResPhi",";#phi_{MC}, rad;#Delta#phi, rad",1e2,-3.15,3.15,1e2,-5e-1,5e-1);
  //-----------------------------------------------------------------------------
  
  TNtuple *nsectors = new TNtuple("nsectors","sectors","thetares:sector");
  TH2I *hnhits = new TH2I("hnhits","# rec hits vs. # sim hits; sim; rec",100,0,100,100,0,100);
  ///
 TNtuple *ntupTrkFit = new TNtuple("ntupTrkFit","Info about reconstructed trks: trk-cand vs. trk after fit","xtc:ytc:ztc:thtc:phitc:xtf:ytf:ztf:thtf:phitf:chi2");

  TH1 *hMCtrkPer = new TH1F("hMCtrkPer","% of MCTrk in one rec.trk",1e3,0,110);
  //Load lumi geo params
  PndLmdDim *lmddim = PndLmdDim::Instance();
  lmddim -> Read_transformation_matrices("/panda/pandaroot/input/trafo_matrices_lmd.dat", false);
  int glBADGEANE=0;
  //  int glBadEv = 0;
  int glNoisehit = 0;// total number of noise hits
  for (Int_t j=0; j<nEvents; j++){
    tTrkRec.GetEntry(j);
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
    tTrkCand.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);
    if(mh>0)
      tHitsMerged.GetEntry(j);
    const int nGeaneTrks = geaneArray->GetEntriesFast();
    const int nParticles = true_tracks->GetEntriesFast();
    const int numTrk = nGeaneTrks;
    const int nRecHits = rechit_array->GetEntriesFast();
    const int nMCHits = true_points->GetEntriesFast();
    
    const int nTrkCandidates = trkcand_array->GetEntriesFast();
    const int nRecTrks = rec_trk->GetEntriesFast();
    if(verboseLevel>0)  
      cout<<"%%%%%! Event #"<<j<<" has "<<nParticles<<" true particles, "<<" out of it "<<nRecHits<<" hits, "<<nTrkCandidates
	  <<" trk-cands, "<<numTrk<<" tracks and "<<nGeaneTrks<<" geane Trks!"<<endl;
    hnRecnMC->Fill(nParticles,nGeaneTrks);
    if(nParticles!=nMCtracks) continue;
    bool secondMC=false;
    for(int imc=0;imc<nParticles;imc++){//check that we don't have any secondaries here
      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(imc);
      int motherID = mctrk->GetMotherID();
      if(motherID>=0){
	if(verboseLevel>0) cout<<"this event contains secondaries, for simplisity skip it."<<endl;
	secondMC=true;
      }
    }
    if(secondMC) continue;
   
    /// Chech how many MC hits has MC trk ----------------------------
    int MCtksSIMhits[nParticles];
    int MCDoubleHits[nParticles];
    for(int imctrk=0;imctrk<nParticles;imctrk++){
      MCtksSIMhits[imctrk]=0;
      MCDoubleHits[imctrk]=0;
    }
    for(int imc=0;imc<nMCHits;imc++){
	  PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(imc));
	  int MCtrk = MCPoint->GetTrackID();
	  MCtksSIMhits[MCtrk]++;
    }
    ///------------------------------------------------------------------------------

    /// Chech how many REC hits has MC trk ----------------------------
    int MCtksREChits[nParticles];
    for(int imctrk=0;imctrk<nParticles;imctrk++){
      MCtksREChits[imctrk]=0;
    }
    if(verboseLevel>7)
      cout<<"    ** ALL REChits are made from MChits: "<<endl;//start MC hit content
    bool fMCnegative = false;
    for(int irec=0;irec<nRecHits;irec++){
	PndSdsMergedHit* myHit = (PndSdsMergedHit*)(rechit_array->At(irec));
	///TODO: check both sides hits, but one need to change rule for missed trk due to hits losses,
	/// since ">2 hit" doesn't work anymore
	int mcrefbot = myHit->GetSecondMCHit();
	int MCtrkidbot,MCtrkidtop;
	if(mcrefbot>0){
	  PndSdsMCPoint* MCPointBot = (PndSdsMCPoint*)(true_points->At(mcrefbot));
	  int MCtrkid = MCPointBot->GetTrackID();
	  if(MCtrkid<0) fMCnegative=true;//TODO: how it is possible???
	  MCtksREChits[MCtrkid]++;
	  MCtrkidbot=MCtrkid;
	  if(verboseLevel>7)
	    cout<<" "<<MCtrkid<<"(MChitID="<<mcrefbot<<",z="<<MCPointBot->GetZ()<<")";
	}
	int mcreftop = myHit->GetRefIndex();
	if(mcreftop>0){
	  PndSdsMCPoint* MCPointTop = (PndSdsMCPoint*)(true_points->At(mcreftop));
	  int MCtrkid = MCPointTop->GetTrackID();
	  if(MCtrkid<0) fMCnegative=true;//TODO: how it is possible???
	  MCtksREChits[MCtrkid]++;
	  MCtrkidtop=MCtrkid;  
	  if(verboseLevel>7)
	    cout<<" "<<MCtrkid<<"(MChitID="<<mcreftop<<",z="<<MCPointTop->GetZ()<<")";
	}
	if(mcreftop>0 && mcrefbot>0){
	  if(MCtrkidtop==MCtrkidbot){
	    MCDoubleHits[MCtrkidbot]++;
	  }
	  else{
	    if(verboseLevel>7)  cout<<"        REChit No."<<irec<<"contain MCid: "<<MCtrkidtop<<", "<<MCtrkidbot<<" !"<<endl;
	  }
	}
	if(verboseLevel>7)
	  cout<<" "<<endl;//next hit content
    }
    if(fMCnegative){
      if(verboseLevel>7) cout<<"bad event, skip it!"<<endl;
    }
    if(fMCnegative) continue;
    ///------------------------------------------------------------------------------


 hnhits->Fill(nMCHits,nRecHits);

    ///Compare trk-cand vs. trk-fit -----------------------------
    // TNtuple *ntupTrkFit = new TNtuple("ntupTrkFit","Info about reconstructed trks: trk-cand vs. trk after fit","xtc:ytc:ztc:thtc:phitc:xtf:ytf:ztf:thtf:phitf:chi2");
    for (Int_t iN=0; iN<nGeaneTrks; iN++){// loop over all reconstructed trks
      PndTrack *trkpnd = (PndTrack*)rec_trk->At(iN);
      double chi2 = trkpnd->GetChi2();
      FairTrackParP fFittedTrkP = trkpnd->GetParamFirst();
      TVector3 PosRecLMD(fFittedTrkP.GetX(),fFittedTrkP.GetY(),fFittedTrkP.GetZ());
      TVector3 MomRecLMD(fFittedTrkP.GetPx(),fFittedTrkP.GetPy(),fFittedTrkP.GetPz());
      MomRecLMD *=Plab/MomRecLMD.Mag();

      int candID = trkpnd->GetRefIndex();
      PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);    
      TVector3 PosRecCandLMD = trkcand->getPosSeed();
      TVector3 MomRecCandLMD = trkcand->getDirSeed();
      MomRecCandLMD *=Plab/MomRecCandLMD.Mag();
      ntupTrkFit->Fill(PosRecCandLMD.X(),PosRecCandLMD.Y(),PosRecCandLMD.Z(),MomRecCandLMD.Theta(),MomRecCandLMD.Phi(),
		       PosRecLMD.X(),PosRecLMD.Y(),PosRecLMD.Z(),MomRecLMD.Theta(),MomRecLMD.Phi(),chi2);
    }
    ///-------------------------------------------------------------------

    //  if(nRecHits<3*nMCtracks) glBadEv++;
    //    double chi2Cont[5*numTrk];
    //    double ndiffIDCont[5*numTrk];
   
    //  if(nTrkCandidates>numTrk) cout<<"Event #"<<j<<" has "<<nTrkCandidates<<" trk-cands and "<<numTrk<<" tracks!"<<endl;



    /// Set MC ID for each track ----------------------------------------------------
    Int_t nRecGEANEtrk = 0;
    int MCtrk[nParticles]; //Number of participation this MCid in rec.tracks
    int RECtrkMCid[nGeaneTrks];//Assignment MC id to REC trk;
    for(int nk=0;nk<nParticles;nk++)
      MCtrk[nk]=0;
    bool goodTrk[nGeaneTrks];
    bool ghostTrk[nGeaneTrks];
    for (Int_t iN=0; iN<nGeaneTrks; iN++){
      goodTrk[iN] = false;
      ghostTrk[iN] = false;
      RECtrkMCid[iN]=-1;
    }

    int goodRectrk=0;//for missed trk-search
    for (Int_t iN=0; iN<nGeaneTrks; iN++){// loop over all reconstructed trks
      //    if(verboseLevel>3)  cout<<"GEANEtrk#"<<iN<<endl;
      
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(iN);
      TVector3 PosRec = fRes->GetPosition();
      // ///get rid from most probably ghost track ------------
      // double pca_lim = 1.;//=10*sigma_Xpca~10*{0.093,0.11,0.12,0.22,0.55};
      // if(Plab<5) pca_lim = 2.;
      // if(Plab<2) pca_lim = 5.;
      // if(fabs(PosRec.X())>pca_lim && fabs(PosRec.Y())>pca_lim) continue; // PCA_x and PCA_y should be < 10sigmaX
      // ///get rid from most probably ghost track (END) ---
      Double_t lyambda = fRes->GetLambda();
      if(lyambda==0){
	cout<<"GEANE didn't propagate "<<iN<<" trk!"<<endl;
	//	cout<<"Event #"<<j<<" diffIDs = "<<diffIDs<<endl;
	glBADGEANE++;
      }
      if(lyambda==0) continue;
     
      PndTrack *trkpnd = (PndTrack*)rec_trk->At(iN);
      // if(verboseLevel>5) 
      // trkpnd->Print();
      double chi2 = trkpnd->GetChi2();
      hchi2->Fill(chi2);
      int candID = trkpnd->GetRefIndex();
      //if(verboseLevel>5) cout<<"candID = "<<candID<<endl;
      PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);    
      const int Ntrkcandhits = trkcand->GetNHits();
      //    PndSdsMCPoint* MCPointHit;
      int MCtrkID[Ntrkcandhits]; //arrray of hits MCid
      for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){ 
	MCtrkID[iHit]=9999;
      }
      Int_t diffIDs=1;
	//Matching between MC & Rec on hits level-----------------------------------
      bool emergExit=false;
      if(verboseLevel>7)
	cout<<"    *** REChits in one trk are made from MChits: "<<endl;//start MC hit content
      for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){ // loop over rec.hits
	PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	Int_t hitID = candhit.GetHitId();
	// if(verboseLevel>5)
	//   cout<<" "<<endl;
	//   cout<<" hitID = "<<hitID<<endl;
	//	PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	PndSdsMergedHit* myHit = (PndSdsMergedHit*)(rechit_array->At(hitID));
	int mcrefbot = myHit->GetSecondMCHit();
	if(mcrefbot>0){
	  // if(verboseLevel>5)
	  //   cout<<" "<<hitID<<"("<<mcrefbot;
	  PndSdsMCPoint* MCPointBot = (PndSdsMCPoint*)(true_points->At(mcrefbot));
	  int MCtrkid = MCPointBot->GetTrackID();
	  if(MCtrkid<0){
	    emergExit=true;
	    break;//TODO: how it is possible???
	  }
	  //	  if(MCtrkid>-1)
	    MCtrkID[iHit]=MCtrkid;
	    if(verboseLevel>7)
	      cout<<" "<<MCtrkid;
	}
	int mcreftop = myHit->GetRefIndex();

	if(mcreftop>0){
	  // if(verboseLevel>5)
	  //   cout<<", "<<mcreftop<<")";
	  PndSdsMCPoint* MCPointTop = (PndSdsMCPoint*)(true_points->At(mcreftop));
	  int MCtrkid = MCPointTop->GetTrackID();
	  if(MCtrkid<0){ 
	    emergExit=true;
	    break;//TODO: how it is possible???
	  }
	  //	  if(MCtrkid>-1)
	    MCtrkID[iHit]=MCtrkid;
	    if(verboseLevel>7)
	      cout<<" "<<MCtrkid;
	}
	if(verboseLevel>7)
	  cout<<" "<<endl;//next hit content
      }
      if(emergExit) continue;
      // if(verboseLevel>5)
      // 	cout<<" "<<endl;

      // if(verboseLevel>3){
      // 	cout<<"Before sorting: ";
      // 	for(Int_t n=0; n<Ntrkcandhits; n++)
      // 	  cout<<" "<<MCtrkID[n];
      // 	cout<<""<<endl;
      // }
      //    if(verboseLevel>1) cout<<""<<endl;
      //   Sorting MC IDs ---------------------------------------- 
      Int_t k, x;
      bool ch=false; //Was element changed? 
      Int_t nch = 0; //How many times?
      for(Int_t n=0; n<Ntrkcandhits; n++) { // n - current position
	k=n; x=MCtrkID[n];
	for(Int_t m=n+1; m<Ntrkcandhits; m++)	// find the least element
	  if (MCtrkID[m]<x){
	    k=m; x=MCtrkID[m];	        // k - index for the least element
	    ch=true; nch++;
	  }
	MCtrkID[k] = MCtrkID[n]; MCtrkID[n] = x; // change position between the least and current elements
      }
      ///--------------------------------------------------------------------------

      // if(verboseLevel>3){
      // 	cout<<"After sorting: ";
      // 	for(Int_t n=0; n<Ntrkcandhits; n++)
      // 	  cout<<" "<<MCtrkID[n];
      // 	cout<<""<<endl;
      // }

      /// Counting number of diff MC ids ----------------------------------------      
      Int_t prevID = MCtrkID[0];
      for(int nk=0;nk<nParticles;nk++){
	if(MCtrkID[0]==nk)
	  MCtrk[nk]=MCtrk[nk]+1;
      }
      diffIDs = 1;
      for(Int_t n=1; n<Ntrkcandhits; n++){ 
	if(MCtrkID[n]>9998) break;
	if(prevID<MCtrkID[n]){
	  diffIDs++;
	  prevID=MCtrkID[n];
	  for(int nk=0;nk<nParticles;nk++){
	    if(MCtrkID[n]==nk)
	      MCtrk[nk]=MCtrk[nk]+1;
	  }
	}
      }
    
      //      ndiffIDCont[iN]=diffIDs;
      hDiffIDs->Fill(diffIDs);
      hntrkcandvsIDs->Fill(diffIDs,nTrkCandidates);
      hnhitsvsIDs->Fill(diffIDs,Ntrkcandhits);
      ///------------------------------------------------------------------------

      /// Set track quality: good or ghost? and assign MC id to rec trk ---------
      if(diffIDs<2){
	RECtrkMCid[iN] = MCtrkID[0];
	goodTrk[iN] = true;
	hMCtrkPer->Fill(100);
      }
      else{
	vector<int> countMC_IDs(diffIDs);
	for(int kn=0;kn<diffIDs;kn++){
	  countMC_IDs[kn]=0;
	}

	prevID = MCtrkID[0];
	int diffCount=0;
	for(Int_t n=0; n<Ntrkcandhits; n++) {
	  if(MCtrkID[n]>9998) break;
	  if(prevID<MCtrkID[n]){
	    diffCount++;
	    prevID=MCtrkID[n];
	  }
	  countMC_IDs[diffCount]++;
	}
	//	if(verboseLevel>1)	cout<<""<<endl;
	int maxID=countMC_IDs[0];
	int posIDmax=0;
	for(int kn=0;kn<diffIDs;kn++){
	  hMCtrkPer->Fill(100*double(countMC_IDs[kn])/Ntrkcandhits);
	  if(verboseLevel>3)  cout<<"countMC_IDs["<<kn<<"]="<<countMC_IDs[kn]<<" 0.65*Ntrkcandhits = "<<0.65*Ntrkcandhits<<endl;
	  if(countMC_IDs[kn]>0.65*Ntrkcandhits){ //more then 65% of hits come from the same MC id
	    goodTrk[iN] = true;
	    ghostTrk[iN] = false;
	  }
	  else{
	    if(!goodTrk[iN]){
	      ghostTrk[iN] = true;
	      //	      cout<<" aaaaa!"<<iN<< " is ghost trk !aaaa"<<endl;
	    }
	  }
	  
	  if(countMC_IDs[kn]>maxID){ 
	    maxID=countMC_IDs[kn];
	    posIDmax = kn;
	  }
	}
	prevID = MCtrkID[0];
	diffCount=0;
	for(Int_t n=0; n<Ntrkcandhits; n++) {
	  if(diffCount==posIDmax) RECtrkMCid[iN] = prevID;
	  if(prevID<MCtrkID[n]){
	    diffCount++;
	    prevID=MCtrkID[n];
	  }
	}
      }
      ///------------------------------------------------------------------------
      
      /// Comporision MC, trk-candidates and REConstructed tracks --------------------------------------------
   
      if(diffIDs>-1){ // All tracks
		
	/// Read MC track parameters ----------------------------------------------------------
	int MCidforREC = RECtrkMCid[iN];
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(MCidforREC);
	Int_t mcID = mctrk->GetPdgCode();
	TVector3 MomMC = mctrk->GetMomentum();
	Double_t thetaMC = MomMC.Theta();
	Double_t phiMC = MomMC.Phi();
	///------------------------------------------------------------------------------------
	
	/// Read track-parameters after back-propagation ---------------------------------------
	//TODO: problem with covarance matrix in FairTrackParH???
	Double_t CovGEANELAB[6][6];
	fRes->GetMARSCov(CovGEANELAB);	
	Double_t errX = fRes->GetDX();
	Double_t errY = fRes->GetDY();
	Double_t errZ = fRes->GetDZ();

	Double_t errPx = fRes->GetDPx();
	Double_t errPy = fRes->GetDPy();
	Double_t errPz = fRes->GetDPz();
	TVector3 errMomBP(errPx,errPy,errPz);

	Double_t thetaBP = TMath::Pi()/2. - lyambda;
	Double_t err_lyambda = fRes->GetDLambda();
	if(err_lyambda==0) err_lyambda = errMomBP.Theta();
	// Double_t err_lyambda = errMom.Theta();
	Double_t phiBP = fRes->GetPhi();
	Double_t err_phi = fRes->GetDPhi();
	if(err_phi==0) err_phi=errMomBP.Phi();
	Double_t errMomRecBP = fRes->GetDQp();
	TVector3 MomRecBP = fRes->GetMomentum();
	Double_t resTheta = thetaBP-thetaMC;
	double resMom = MomMC.Mag()-MomRecBP.Mag();
	hgoodTheta->Fill(thetaMC);
	hgoodPhi->Fill(phiMC);
	hgoodPhiTheta->Fill(thetaMC,phiMC);
	hMCThetaResTheta->Fill(thetaMC,thetaMC-thetaBP);
	hMCPhiResPhi->Fill(phiMC,phiMC-phiBP);
	hResPointX->Fill(-PosRec.X());
	hResPointY->Fill(-PosRec.Y());
	hResPointZ->Fill(-PosRec.Z());
	hErrPointX->Fill(errX);
	hErrPointY->Fill(errY);
	hErrPointZ->Fill(errZ);
	hPullPointX->Fill(-PosRec.X()/errX);
	hPullPointY->Fill(-PosRec.Y()/errY);
	hPullPointZ->Fill(-PosRec.Z()/errZ);
	hResPointPx->Fill(MomMC.X()-MomRecBP.X());
	hResPointPy->Fill(MomMC.Y()-MomRecBP.Y());
	hResPointPz->Fill(MomMC.Z()-MomRecBP.Z());
	hErrPointPx->Fill(errPx);
	hPullPointPx->Fill((MomMC.X()-MomRecBP.X())/errPx);
	hErrPointPy->Fill(errPy);
	hPullPointPy->Fill((MomMC.Y()-MomRecBP.Y())/errPy);
	hErrPointPz->Fill(errPz);
	hPullPointPz->Fill((MomMC.Z()-MomRecBP.Z())/errPz);

	Double_t resPhi = phiBP-phiMC;
	hResMom->Fill(resMom);
	hErrMom->Fill(errMomRecBP);
	hPullMom->Fill(resMom/errMomRecBP);
	hResTheta->Fill(resTheta);
	hErrTheta->Fill(err_lyambda);
	hPullTheta->Fill(resTheta/err_lyambda);
	hResPhi->Fill(resPhi);
	hErrPhi->Fill(err_phi);
	hPullPhi->Fill(resPhi/err_phi);

	///Let's checked for good tracks by phi\theta definition ------------------

	for(Int_t jk=0; jk< nParticles;jk++){
	  PndMCTrack *mctrk0 =(PndMCTrack*) true_tracks->At(jk);
	  Int_t mcID0 = mctrk0->GetPdgCode();
	  if(mcID0==-2212){
	    TVector3 MomMC0 = mctrk0->GetMomentum();
	    double diffTheta=fabs(MomMC0.Theta()-thetaBP)/err_lyambda;
	    double diffPhi=fabs(MomMC0.Phi()-phiBP)/err_phi;

	    if(fabs(MomMC0.Phi())<0.14 || fabs(MomMC0.Phi())>3){ //for #phi near edge reconstructed angle can differ
	      double phiMC1 = -MomMC0.Phi();
	      double diffPhi2=fabs(phiMC1-phiBP)/err_phi;
	      if(diffPhi2<diffPhi) diffPhi = diffPhi2;
	    }
	
	    hntrkmissedPhiTheta->Fill(diffTheta,diffPhi);
	    if(diffTheta<4. && diffPhi<4.) goodRectrk++;
	  }
	}
	///(end) check for good tracks --------------------------
	///==================================
      }
      ///-------------------------------------------------------------------------------------
      //  MCtrkID.clear();
    }
  
    // /// (I) missed\ghost tracks are defined on Phi\Theta difference between MC&REC trks
    // hntrkgood_I->Fill(goodRectrk);
    // //    if((nMCtracks-goodRectrk)>0) hntrkmissed_I->Fill(nMCtracks-goodRectrk);
    // if((nGeaneTrks-goodRectrk)>0) hntrkghost_I->Fill(nGeaneTrks-goodRectrk);
    // if(verboseLevel>0){
    // cout<<nMCtracks<<" trks per event were simulated and "<<nGeaneTrks<<" were reconstructed"<<endl;
    // // cout<<"-- (I) dPhi&dTheta -------------------------"<<endl;
    // // cout<<"Good trks: "<<goodRectrk<<" missed: "<<nMCtracks-goodRectrk<<" ghost: "<<nGeaneTrks-goodRectrk<<endl;
    // // cout<<"--------------------------------"<<endl;
    // }


    /// (II) missed\ghost tracks are defined on hits information
    /// GHOST ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    ///check repeated assignment to one MC trk (maybe one MC trk rec. trk was assigned  to several REC trk) ---------------------------------
    //NB: it's only quantity and _ NOT_ quality check. 1st REC trk will be called GOOD and 2nd and others GHOST
    int RECassigMC[nGeaneTrks];//how many time MCid from this RECtrk met in other RECtrks
    for (Int_t iN=0; iN<nGeaneTrks; iN++)
      RECassigMC[iN]=1;
    
    for (Int_t iN=0; iN<nGeaneTrks; iN++){
      int mc_i = RECtrkMCid[iN];
      for (Int_t jN=(iN+1); jN<nGeaneTrks; jN++){
	int mc_j = RECtrkMCid[jN];
	if(mc_i==mc_j){
	  RECassigMC[iN]++;
	  if(verboseLevel>3) cout<<"GHOST?!: rec.trks #"<<iN<<" and #"<<jN<< "were assigned to one MC trk =|"<<endl;
	}
      }
    }
    ///END check repeated assignment to one MC trk ------------------------------------------------------------------------------------------------------------------

    int goodRecII=0, ghostRecI=0, ghostRecII=0;
    for (Int_t iN=0; iN<nGeaneTrks; iN++){ //ghost/good trk determination
      int trkType = -1;
      if(RECassigMC[iN]>1){ //one RECtrk was already called GOOD for this MCid -> all next are GHOSTs
	goodTrk[iN]=false;
	ghostTrk[iN]=true;
	trkType = +2;
	ghostRecI++;
      }

     
      if(goodTrk[iN]){
	if(verboseLevel>7) cout<<"... RECtrk#"<<iN<<" was defined as GOOD ..."<<endl;
	goodRecII++;
	trkType = 0;
      }
      if(ghostTrk[iN]){
	if(verboseLevel>7) cout<<"... RECtrk#"<<iN<<" was defined as GHOST ..."<<endl;
	if(trkType<0){
	  trkType = +1;
	  ghostRecII++;
	}
      }
      if(trkType<0) 
	cout<<"Ooops, RECtrk isn't GOOD and isn't GHOST!!!"<<endl;
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(iN);
      Double_t lyambda = fRes->GetLambda();
      Double_t thetaBP = TMath::Pi()/2. - lyambda;
      Double_t phiBP = fRes->GetPhi();
      TVector3 MomRecBP = fRes->GetMomentum();
      TVector3 PosBP = fRes->GetPosition();
      ntuprecTrk->Fill(PosBP.X(),PosBP.Y(),PosBP.Z(),MomRecBP.Mag(),thetaBP,phiBP,trkType);
    }
    hntrkgood_II->Fill(goodRecII);
    if(ghostRecII>0) hntrkghost_II->Fill(ghostRecII);
    if(ghostRecI>0) hntrkghost_I->Fill(ghostRecI);
    ///END GHOST--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    /// MISSED ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if((nMCtracks-goodRecII)>0){ //missed trks
     
      int nMCmissedTrkSearch=0;
      int nMCmissedLossHits=0;
      for(int imc=0;imc<nParticles;imc++){//MC trks
	bool missTrk=true;
	for(int irec=0;irec<nGeaneTrks;irec++){//RECids assigment
	  int mc_comp = RECtrkMCid[irec];
	  if(mc_comp==imc) missTrk=false;
	}
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(imc);
	TVector3 MomMC = mctrk->GetMomentum();
	TVector3 PosMC = mctrk->GetStartVertex();
	int trkQ=0;
	/// if MC trk was missed, justify why 
	int minHits = MCDoubleHits[imc]+2;
	if(MCDoubleHits[imc]==4) minHits -=1; 
	// if(MCDoubleHits[imc]>0){// track contains double hits
	//   minHits = MCDoubleHits[imc]+3;
	//   // minHits = MCDoubleHits[imc]*2+1;
	//   // if(MCDoubleHits[imc]<2){//only one plane contains double hits,
	//   //   minHits = 3;//to avoid situation like this:  *- ** -- --
	//   //   //this should be fine: ** *- *- *-
	//   // }
	//   // else{
	//   //   if(MCDoubleHits[imc]==2){//only two planes contains double hits
	//   //     minHits = 4;// to adoid situation like: ** ** -- -- .
	//   //   }
	//   //   else{ // 3 and more planes contains double hits
	//   //     minHits = 4;
	//   //   }
	//   // }
	// }

	if(missTrk){
	  if(MCtksREChits[imc]>minHits){//missed during trk-search
	    if(verboseLevel>7){
	      cout<<" --- MCtrk#"<<imc<<" was defined as MISSED during trk-search (#MChits="<<MCtksREChits[imc]
				   <<" with limit>"<<minHits<<")"<<endl;
	      if(MCDoubleHits[imc]>0) cout<<"         NB:this MCtrk contains "<<MCDoubleHits[imc]<<" double hits!"<<endl;
	    }
	    trkQ=-1;
	    nMCmissedTrkSearch++;
	  }
	  else{//missed due to small amount of hits
	    nMCmissedLossHits++;
	    trkQ=-2;
	    if(verboseLevel>7) cout<<" --- MCtrk#"<<imc<<" was defined as MISSED due to little amount of hits (#MChits="
				   <<MCtksREChits[imc]<<" with limit>"<<minHits<<")"<<endl;
	  }
	  ntupMCTrk->Fill(PosMC.X(),PosMC.Y(),PosMC.Z(),MomMC.Mag(),MomMC.Theta(),MomMC.Phi(),trkQ,MCtksSIMhits[imc],MCtksREChits[imc],MCDoubleHits[imc]);
	  //	  cout<<"trk was marked ad missed, reason trkQ="<<trkQ<<endl;
	}
	//	if(verboseLevel>3) cout<<"#REChits = "<<MCtksREChits[imc]<<endl;
      }
      if(nMCmissedTrkSearch>0) hntrkmissed_II->Fill(nMCmissedTrkSearch); //missed by track search only and not because it wasn't enought hits!
      if(nMCmissedLossHits>0) hntrkmissed_I->Fill(nMCmissedLossHits);//missed during hit rec
    }
    else{ //fill ntuple with recontruted MCtrks
      for(int imc=0;imc<nParticles;imc++){//MC trks
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(imc);
	TVector3 MomMC = mctrk->GetMomentum();
	TVector3 PosMC = mctrk->GetStartVertex();
	int trkQ = 0;
	ntupMCTrk->Fill(PosMC.X(),PosMC.Y(),PosMC.Z(),MomMC.Mag(),MomMC.Theta(),MomMC.Phi(),trkQ,MCtksSIMhits[imc],MCtksREChits[imc],MCDoubleHits[imc]);
      }
    }
    /// END MISSED -----------------------------------------------------------------------------------------------------------------------------------------------------------------

    if(verboseLevel>0){
      if((nMCtracks-goodRecII)>0 || ghostRecII>0){//TEST only about missed or ghost trks
	cout<<"-- Ev#"<<j<<endl;
	cout<<"-- (II) Hits matching -------------------------"<<endl;
	cout<<"Good trks: "<<goodRecII<<" missed: "<<nMCtracks-goodRecII<<" ghost: "<<ghostRecII<<endl;
	cout<<"--------------------------------"<<endl;
	cout<<" "<<endl;
      }
    }
    //  cout<<"number of good: "<<goodRecII<<" number of missed: "<<nMCtracks-goodRecII<<" number of ghost: "<<ghostRecII<<endl;
    ///-------------------------------------------------------------------------------------
  
    //    cout<<"nGeaneTrks = "<<nGeaneTrks<<" nRecGEANEtrk = "<<nRecGEANEtrk<<endl;
    hntrk->Fill(nRecGEANEtrk);
    ///-------------------------------------------------------------------------------------------
    //  cout<<"========="<<endl;
    for(Int_t jk=0; jk< nParticles;jk++){
      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(jk);
      Int_t mcID = mctrk->GetPdgCode();
      if(mcID==-2212){
	TVector3 MomMC = mctrk->GetMomentum();
	hallTheta->Fill(MomMC.Theta());
	hallPhi->Fill(MomMC.Phi());
	hallPhiTheta->Fill(MomMC.Theta(),MomMC.Phi());
      }
    }
  }
  
  TCanvas *c1 = new TCanvas("IDsINFO");
  c1->Divide(2,2);
  c1->cd(1);
  hnRecnMC->Draw("colz");
  c1->cd(2);
  hntrkcand->Draw();
  hntrk->SetLineColor(2);
  hntrk->Draw("same");
  c1->cd(3);
  hDiffIDs->Draw();
  c1->cd(4);
  hnhitsvsIDs->Draw("colz");
  c1->Write();
  c1->Close();
  
  TH1F *heffTheta = (TH1F*)hgoodTheta->Clone("heffTheta");
  heffTheta->SetTitle("Efficiency vs. #theta");
  heffTheta->Divide(hallTheta);
  TH1F *heffPhi = (TH1F*)hgoodPhi->Clone("heffPhi");
  heffPhi->SetTitle("Efficiency vs. #phi");
  heffPhi->Divide(hallPhi);

  TH2F *heffPhiTheta = (TH2F*)hgoodPhiTheta->Clone("heffPhiTheta");
  heffPhiTheta->SetTitle("Efficiency #phi & #theta");
  heffPhiTheta->Divide(hallPhiTheta);

  TCanvas *c2 = new TCanvas("EffINFO");
  c2->Divide(3,2);
  c2->cd(1);
  hallTheta->SetLineWidth(2);
  hallTheta->SetFillStyle(3354);
  hallTheta->SetFillColor(kBlack);
  hallTheta->Draw();
  hgoodTheta->SetLineWidth(2);
  hgoodTheta->SetLineColor(2);
  hgoodTheta->SetFillStyle(3690);
  hgoodTheta->SetFillColor(kRed);
  hgoodTheta->Draw("same");
  c2->cd(4);
  heffTheta->SetMinimum(0.9);
  heffTheta->Draw();
  c2->cd(3);
  hallPhiTheta->Draw("colz");
  c2->cd(2);
  hallPhi->SetLineWidth(2);
  hallPhi->SetFillStyle(3354);
  hallPhi->SetFillColor(kBlack);
  hallPhi->Draw();
  hgoodPhi->SetLineWidth(2);
  hgoodPhi->SetLineColor(2);
  hgoodPhi->SetFillColor(kRed);
  hgoodPhi->SetFillStyle(3690);
  hgoodPhi->Draw("same");
  c2->cd(5);
  heffPhi->SetMinimum(0.9);
  heffPhi->Draw();
  c2->cd(6);
  heffPhiTheta->Draw("colz");

  c2->Write();
  c2->Close();

  TF1 *funrp = new TF1("fitrp","gaus",-1e-5,1e-5);
  funrp->SetParameters(100,0,3e-3);
  funrp->SetParNames("Constant","Mean","Sigma");
  
  TF1 *funrth = new TF1("fitrth","gaus",-0.01,0.01);
  funrth->SetParameters(100,0,3e-3);
  funrth->SetParNames("Constant","Mean","Sigma");
  
  TF1 *funrphi = new TF1("fitrphi","gaus",-1,1);
  funrphi->SetParameters(100,0,3e-3);
  funrphi->SetParNames("Constant","Mean","Sigma");
  
  TF1 *funp = new TF1("fitp","gaus",-20,20);
  funp->SetParameters(100,0,3e-3);
  funp->SetParNames("Constant","Mean","Sigma");
  
  TCanvas *c3 = new TCanvas("ResMomINFO");
  c3->Divide(3,3);
  c3->cd(1);
  hResMom->Fit(funrp,"r");
  hResMom->Draw();
  c3->cd(2);
  hResTheta->Fit(funrth,"r");
  hResTheta->Draw();
  c3->cd(3);
  hResPhi->Fit(funrphi,"r");
  hResPhi->Draw();
  c3->cd(4);
  hPullMom->Draw();
  c3->cd(5);
  hPullTheta->Fit(funp,"r");
  hPullTheta->Draw();
  c3->cd(6);
  hPullPhi->Fit(funp,"r");
  hPullPhi->Draw();
  c3->cd(7);
  hErrMom->Draw();
  c3->cd(8);
  hErrTheta->Draw();
  c3->cd(9);
  hErrPhi->Draw();

  c3->Write();
  c3->Close();

  
  TF1 *funcoord = new TF1("fitcoord","gaus",-1,1.);
  funcoord->SetParameters(1e4,0,1e-1);
  funcoord->SetParNames("Constant","Mean","Sigma");
  
  // TF1 *funp = new TF1("fitp","gaus",-10,10);
  funp->SetParameters(100,0,1);
  funp->SetParNames("Constant","Mean","Sigma");

  TCanvas *c4 = new TCanvas("ResPCAINFO");
  c4->Divide(3,3);
  c4->cd(1);
  hResPointX->Fit(funcoord,"r");
  hResPointX->Draw();
  c4->cd(2);
  hResPointY->Fit(funcoord,"r");
  hResPointY->Draw();
  c4->cd(3);
  hResPointZ->Fit(funcoord,"r");
  hResPointZ->Draw();
  c4->cd(4);
  hErrPointX->Draw();
  c4->cd(5);
  hErrPointY->Draw();
  c4->cd(6);
  hErrPointZ->Draw();
  c4->cd(7);
  hPullPointX->Fit(funp,"r");
  hPullPointX->Draw();
  c4->cd(8);
  hPullPointY->Fit(funp,"r");
  hPullPointY->Draw();
  c4->cd(9);
  hPullPointZ->Fit(funp,"r");
  hPullPointZ->Draw();
  c4->Write();
  c4->Close();

  TCanvas *c42 = new TCanvas("ResMomPointINFO");
  c42->Divide(3,3);
  c42->cd(1);
  hResPointPx->Fit(funcoord,"r");
  hResPointPx->Draw();
  c42->cd(2);
  hResPointPy->Fit(funcoord,"r");
  hResPointPy->Draw();
  c42->cd(3);
  hResPointPz->Fit(funcoord,"r");
  hResPointPz->Draw();
  c42->cd(4);
  hErrPointPx->Draw();
  c42->cd(5);
  hErrPointPy->Draw();
  c42->cd(6);
  hErrPointPz->Draw();
  c42->cd(7);
  hPullPointPx->Fit(funp,"r");
  hPullPointPx->Draw();
  c42->cd(8);
  hPullPointPy->Fit(funp,"r");
  hPullPointPy->Draw();
  c42->cd(9);
  hPullPointPz->Fit(funp,"r");
  hPullPointPz->Draw();
  c42->Write();
  c42->Close();

  hchi2->Write();
  heffTheta->Write();
  heffPhi->Write();

  hnRecnMC->Write();
  hDiffIDs->Write();
  hntrkcand->Write();
  hntrk->Write();
  hntrkcandvsIDs->Write();

  hResMom->Write();
  hErrMom->Write();
  hPullMom->Write();
  hResPhi->Write();
  hErrPhi->Write();
  hPullPhi->Write();
  hResTheta->Write();
  hErrTheta->Write();
  hPullTheta->Write();
  
  TCanvas *c10 = new TCanvas("ThetaPhiDistr");
  c10->Divide(4,2);
  c10->cd(1);
  hMCThetaResTheta->Draw("colz");
  c10->cd(5);
  hMCPhiResPhi->Draw("colz");

  TH1F *fdiffThetaMean;
  TH1F *fdiffThetaSigma;
  TH1F *fdiffThetaChi2;
  hMCThetaResTheta->FitSlicesY();
  fdiffThetaMean = (TH1F*)gDirectory->Get("hMCThetaResTheta_1");
  fdiffThetaSigma = (TH1F*)gDirectory->Get("hMCThetaResTheta_2");
  fdiffThetaChi2= (TH1F*)gDirectory->Get("hMCThetaResTheta_chi2");
  TH1F *fdiffPhiMean;
  TH1F *fdiffPhiSigma;
  TH1F *fdiffPhiChi2;
  hMCPhiResPhi->FitSlicesY();
  fdiffPhiMean = (TH1F*)gDirectory->Get("hMCPhiResPhi_1");
  fdiffPhiSigma = (TH1F*)gDirectory->Get("hMCPhiResPhi_2");
  fdiffPhiChi2 = (TH1F*)gDirectory->Get("hMCPhiResPhi_chi2");
  c10->cd(2);
  fdiffThetaMean->Draw();
  c10->cd(6);
  fdiffPhiMean->Draw();
  c10->cd(3);
  fdiffThetaSigma->Draw();
  c10->cd(7);
  fdiffPhiSigma->Draw();
  c10->cd(4);
  fdiffThetaChi2->Draw();
  c10->cd(8);
  fdiffPhiChi2->Draw();
  c10->Write();
  c10->Close();

  
  hMCThetaResTheta->Write();
  hMCPhiResPhi->Write();
  
  
  hResPointPx->Write();
  hErrPointPx->Write();
  hPullPointPx->Write();
  hResPointPy->Write();
  hErrPointPy->Write();
  hPullPointPy->Write();
  hResPointPz->Write();
  hErrPointPz->Write();
  hPullPointPz->Write();

  hResPointX->Write();
  hErrPointX->Write();
  hPullPointX->Write();
  hResPointY->Write();
  hErrPointY->Write();
  hPullPointY->Write();
  hResPointZ->Write();
  hErrPointZ->Write();
  hPullPointZ->Write();

  hnhitsvsIDs->Write();

 hntrkmissedPhiTheta->Write();
 hntrkmissed_I->Write();
 hntrkghost_I->Write();
 hntrkmissed_II->Write();
 hntrkghost_II->Write();
 // hntrkgood_I->Write();
 hntrkgood_II->Write();
 heffPhiTheta->Write();
 ntuprecTrk->Write();
 ntupMCTrk->Write();
 hnhits->Write();
 nsectors->Write();
 hMCtrkPer->Write();
 ntupTrkFit->Write();
 f->Close();
 // cout<<"Number of events with low number of hits (less then 3 per trk): "<<glBadEv<<endl;
 cout<<"Number of trks where GEANE failed: "<<glBADGEANE<<endl;
 cout<<"Total number of noise hits = "<<glNoisehit<<endl;
}
