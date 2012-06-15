//################################################################
//# Macros for Background\Signal study
//# !need all files from full reconstruction chain (results from GEANE, MC etc.)
//# author: Anastasia Karavdina
//# date: April, 2012
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(bkg_rec_mc_match BkgStudies_MCmatch_withSignal_exe.C)
//# target_link_libraries(bkg_rec_mc_match ${ROOT_LIBRARIES} Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./bkg_rec_mc_match --help
//################################################################

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
#include<TRotation.h>
#include<TVector3.h>
#include<TMath.h>
#include<TGaxis.h>
#include<TNtuple.h>

#include<TStopwatch.h>

#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>

//lmd track
#include<PndLinTrack.h>
#include<PndTrackCand.h>
#include<PndTrackCandHit.h>
#include<PndSdsHit.h>
#include<PndSdsClusterStrip.h>
#include<PndSdsDigiStrip.h>

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
  bool fillBkgTree = true;
  //TODO: read this like params!
  //  const int nEvents=500000;
  int nEvents=200000;
  int startEvent=0;
  TString storePath="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutputBkg";
  double Plab=15.;
  int verboseLevel=0;
  std::string startStr="", momStr="", nStr="", pathStr="", verbStr="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    
    std::cout << "This is Background study script with parameters\n"
	      <<"-s start event \n"
	      <<"-n Number of events \n"
	      <<"-mom Beam Momentum \n"
	      <<"-path path to the file(s)"
	      <<"-v verbose Level (if>0, print out some information)"
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
   
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
  }

  std::stringstream startSStr(startStr), momSStr(momStr), nSStr(nStr), pathSStr(pathStr), verbSStr(verbStr);

  startSStr >> startEvent;
  momSStr >> Plab;
  nSStr >> nEvents;
  pathSStr >> storePath;
  verbSStr >> verboseLevel;

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
  
  TString recHit=storePath+"/Lumi_reco_";
  recHit += startEvent;
  recHit += ".root";
  TChain tHits("cbmsim");
  tHits.Add(recHit);
  
  TString trkCand = storePath+"/Lumi_TCand_";
  trkCand += startEvent;
  trkCand += ".root";
  TChain tTrkCand("cbmsim");
  tTrkCand.Add(trkCand);
  
  TString recTrack = storePath+"/Lumi_Track_";
  recTrack += startEvent;
  recTrack += ".root";
  TChain tTrkRec("cbmsim");
  tTrkRec.Add(recTrack);
 
  TString geaneFile = storePath+"/Lumi_Geane_";
  geaneFile += startEvent;
  geaneFile += ".root";
  TChain tgeane("cbmsim");
  tgeane.Add(geaneFile);
  
  // ---------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/compMC_and_REC_withSignal";
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
  TClonesArray* fStripClusterArray = new TClonesArray("PndSdsClusterStrip");
  tHits.SetBranchAddress("LMDStripClusterCand",&fStripClusterArray); 
  
  TClonesArray* fStripDigiArray = new TClonesArray("PndSdsDigiStrip");
  tdigiHits.SetBranchAddress("LMDStripDigis",&fStripDigiArray); 
  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  tHits.SetBranchAddress("LMDHitsStrip",&rechit_array);  //Points for Tracks
  //----------------------------------------------------------------------------------
  
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  //----------------------------------------------------------------------------------  

  //--- Geane info ------------------------------------------------------------------
  TClonesArray* geaneArray =new TClonesArray("FairTrackParH");
  tgeane.SetBranchAddress("GeaneTrackFinal",&geaneArray);  //Tracks with parabolic parametrisation
  
  //--- Output histogram -----------------------------------------------------
  //Vertex
  TH1 *hVxRec = new TH1F("hVxRec","Vx^{Rec};Vx, cm;",1e3,-2e1,2e1);
  TH1 *hVyRec = new TH1F("hVyRec","Vy^{Rec};Vy, cm;",1e3,-2e1,2e1);
  TH1 *hVzRec = new TH1F("hVzRec","Vz^{Rec};Vz, cm;",1e3,-2e1,2e1);

  TH1 *hVxRecAP = new TH1F("hVxRecAP","Vx^{Rec}_{#bar{p}};Vx, cm;",1e3,-2e1,2e1);
  TH1 *hVyRecAP = new TH1F("hVyRecAP","Vy^{Rec}_{#bar{p}};Vy, cm;",1e3,-2e1,2e1);
  TH1 *hVzRecAP = new TH1F("hVzRecAP","Vz^{Rec}_{#bar{p}};Vz, cm;",1e3,-2e1,2e1);

  TH1 *hVxMC_all = new TH1F("hVxMC_all","Vx^{MCall};Vx, cm;",1e3,-2e1,2e1);
  TH1 *hVyMC_all = new TH1F("hVyMC_all","Vy^{MCall};Vy, cm;",1e3,-2e1,2e1);
  TH1 *hVzMC_all = new TH1F("hVzMC_all","Vz^{MCall};Vz, cm;",1e3,-2e1,2e1);

  TH1 *hVxMC = new TH1F("hVxMC","Vx^{MC};Vx, cm;",1e3,-2e1,2e1);
  TH1 *hVyMC = new TH1F("hVyMC","Vy^{MC};Vy, cm;",1e3,-2e1,2e1);
  TH1 *hVzMC = new TH1F("hVzMC","Vz^{MC};Vz, cm;",1e3,-2e1,2e1);

  TH1 *hVxMCAP = new TH1F("hVxMCAP","Vx^{MC}_{#bar{p}};Vx, cm;",1e3,-2e1,2e1);
  TH1 *hVyMCAP = new TH1F("hVyMCAP","Vy^{MC}_{#bar{p}};Vy, cm;",1e3,-2e1,2e1);
  TH1 *hVzMCAP = new TH1F("hVzMCAP","Vz^{MC}_{#bar{p}};Vz, cm;",1e3,-2e1,2e1);

  //Momentum
  TH1 *hPxRec = new TH1F("hPxRec","Px^{Rec};Px, GeV/c;",1e3,-5,5);
  TH1 *hPyRec = new TH1F("hPyRec","Py^{Rec};Py, GeV/c;",1e3,-5,5);
  TH1 *hPzRec = new TH1F("hPzRec","Pz^{Rec};Pz, GeV/c;",1e3,-2e1,2e1);

  TH1 *hPxRecAP = new TH1F("hPxRecAP","Px^{Rec}_{#bar{p}};Px, GeV/c;",1e3,-5,5);
  TH1 *hPyRecAP = new TH1F("hPyRecAP","Py^{Rec}_{#bar{p}};Py, GeV/c;",1e3,-5,5);
  TH1 *hPzRecAP = new TH1F("hPzRecAP","Pz^{Rec}_{#bar{p}};Pz, GeV/c;",1e3,-2e1,2e1);

  TH1 *hPxMC_all = new TH1F("hPxMC_all","Px^{MCall};Px, GeV/c;",1e3,-1e1,1e1);
  TH1 *hPyMC_all = new TH1F("hPyMC_all","Py^{MCall};Py, GeV/c;",1e3,-1e1,1e1);
  TH1 *hPzMC_all = new TH1F("hPzMC_all","Pz^{MCall};Pz, GeV/c;",1e3,-5e1,5e1);

  TH1 *hPxMC = new TH1F("hPxMC","Px^{MC};Px, GeV/c;",1e3,-5,5);
  TH1 *hPyMC = new TH1F("hPyMC","Py^{MC};Py, GeV/c;",1e3,-5,5);
  TH1 *hPzMC = new TH1F("hPzMC","Pz^{MC};Pz, GeV/c;",1e3,-2e1,2e1);


  TH1 *hPRec = new TH1F("hPRec","|P|^{Rec};|P|, GeV/c;",1e3,0,5e1);
  TH1 *hPhiRec = new TH1F("hPhiRec","#phi^{Rec};#phi, rad;",1e3,-3.15,3.15);
  TH1 *hThetaRec = new TH1F("hThetaRec","#theta^{Rec};#theta, rad;",1e3,0,0.1);

  TH1 *hPRecAP = new TH1F("hPRecAP","|P|^{Rec}_{#bar{p}};|P|, GeV/c;",1e3,0,5e1);
  TH1 *hPhiRecAP = new TH1F("hPhiRecAP","#phi^{Rec}_{#bar{p}};#phi, rad;",1e3,-3.15,3.15);
  TH1 *hThetaRecAP = new TH1F("hThetaRecAP","#theta^{Rec}_{#bar{p}};#theta, rad;",1e3,0,0.1);
  TH1 *hThetaRecAPnoise = new TH1F("hThetaRecAPnoise","#theta^{Rec}_{#bar{p}} with <<noise>> hits;#theta, rad;",1e3,0,0.1);

  TH1 *hPMC_all = new TH1F("hPMC_all","|P|^{MCall};|P|, GeV/c;",1e3,0,5e1);
  TH1 *hThetaMC_all = new TH1F("hThetaMC_all","#theta^{MCall};#theta, rad;",1e3,0,3.15);
  TH1 *hPhiMC_all = new TH1F("hPhiMC_all","#phi^{MCall};#phi, rad;",1e3,-3.15,3.15);

  TH1 *hPMC = new TH1F("hPMC","|P|^{MC};|P|, GeV/c;",1e3,0,5e1);
  TH1 *hThetaMC = new TH1F("hThetaMC","#theta^{MC};#theta, rad;",1e3,0,0.1);
  TH1 *hPhiMC = new TH1F("hPhiMC","#phi^{MC};#phi, rad;",1e3,-3.15,3.15);

  TH1 *hPMCAP = new TH1F("hPMCAP","|P|^{MC}_{#bar{p}};|P|, GeV/c;",1e3,0,5e1);
  TH1 *hThetaMCAP = new TH1F("hThetaMCAP","#theta^{MC}_{#bar{p}};#theta, rad;",1e3,0,0.1);
  TH1 *hPhiMCAP = new TH1F("hPhiMCAP","#phi^{MC}_{#bar{p}};#phi, rad;",1e3,-3.15,3.15);

  //PDG ID
  TH1 *hMCpdg_all = new TH1F("hMCpdg_all","PDG^{ALL}_{id};",1e4,-5e3,5e3);
  TH1 *hMCpdg = new TH1F("hMCpdg","PDG_{id};",1e4,-5e3,5e3);
  TH1 *hMCpdgSUM = new TH1F("hMCpdgSUM","sum of PDG_{id} for events with reconstructed trks;",1e4,0,1e4);
  TH2 *hMCpdgSUM_num = new TH2F("hMCpdgSUM_num","sum of PDG_{id} for events with reconstructed trks;pdgSUM;#trks",1e4,0,1e4,20,0,20);
  //Number of diff. trks
  TH1 *hMCtrk = new TH1F("hMCtrk","Participation of diff MC.Trk in one Rec.Trk;Number of MC trks",10,0,10);
  //Number of diff. trks
  TH1 *hRecNtrk = new TH1F("hRecNtrk","Number of Rec.Trk;# trks",10,0,10);
  //Number of diff. trks vs. number of hits from this trk
  TH2 *hMCtrkNhits = new TH2I("hMCtrkNhits","Participation of diff MC.Trk in one Rec.Trk vs. Number of hits;Number of hits; ID of MC trk",10,0,10,7e3,-3.5e3,3.5e3);
  TH2 *hMCtrkPhits = new TH2D("hMCtrkPhits","Participation of diff MC.Trk in one Rec.Trk vs. % of hits;% of hits; ID of MC trk",110,0,110.,7e3,-3.5e3,3.5e3);

  TTree *tTheta;
  TTree *tThetaMC = new TTree("tThetaMC", "#theta_{MC}");
  Double_t thetaMC;
  tThetaMC->Branch("thetaMC", &thetaMC);

  TNtuple *nrecall = new TNtuple("nrecall","recAll","x:y:z:px:py:pz:p:theta:phi");
  TNtuple *nrecsig = new TNtuple("nrecsig","recSig","id:x:y:z:px:py:pz:p:theta:phi");
  TNtuple *nrecbkg = new TNtuple("nrecbkg","recBkg","id:sumid:x:y:z:px:py:pz:p:theta:phi");
  TNtuple *nmcall = new TNtuple("nmcall","mcAll","id:sumid:x:y:z:px:py:pz:p:theta:phi");
  for (Int_t j=0; j<nEvents; j++){
  //for (Int_t j=9000; j<nEvents; j++){

    double TotCharge = 0;
    if(verboseLevel>2)  cout<<"Event #"<<j<<endl;

    // Read GEANE & MC tree -----------------------------------------------------------------
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
    tTrkCand.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);

    const int nGeaneTrks = geaneArray->GetEntriesFast();
    const int nParticles = true_tracks->GetEntriesFast();
    const int numTrk = rec_trk->GetEntriesFast();
    hRecNtrk->Fill(nGeaneTrks);
    if(verboseLevel>2)  cout<<"Number of sim.particles: "<<nParticles<<" number of rec.trks: "<<numTrk<<" number of geane trks: "<<nGeaneTrks<<endl;
    // --------------------------------------------------------------------------------------
    // if(nGeaneTrks==0) continue;

 /// Read MC track parameters ---------------------------------------------------------------
    int sumID=0;
    for (Int_t iN=0; iN<nParticles; iN++){
      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(iN);
      Int_t mcID = mctrk->GetPdgCode();
      if(verboseLevel==1){
	if(nGeaneTrks!=0){
	  if(nParticles==2 && fabs(mcID)==2212){}
	  else cout<<" "<<mcID;
	}
      }
      sumID+=fabs(mcID);
      if(verboseLevel>2)   cout<<" sim.Trk #"<<iN<<" PDGid = "<<mcID<<endl;
      if(fabs(mcID)>1e4) continue;
    
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(mcID);
      Double_t  fCharge = fParticle->Charge();
      TotCharge += fCharge;
      hMCpdg_all->Fill(mcID);
      //	  if(mcID!=-2212) continue;
      TVector3 MomMC_all = mctrk->GetMomentum();
      hPMC_all->Fill(MomMC_all.Mag());
      hPxMC_all->Fill(MomMC_all.X());
      hPyMC_all->Fill(MomMC_all.Y());
      hPzMC_all->Fill(MomMC_all.Z());
      Double_t thetaMC_all = MomMC_all.Theta();
      hThetaMC_all->Fill(thetaMC_all);
      Double_t phiMC_all = MomMC_all.Phi();
      hPhiMC_all->Fill(phiMC_all);
      TVector3 StartMC_all = mctrk->GetStartVertex();
      hVxMC_all->Fill(StartMC_all.X());
      hVyMC_all->Fill(StartMC_all.Y());
      hVzMC_all->Fill(StartMC_all.Z());
    }
    if(TotCharge!=0){
      cout<<"For Event #"<<j<<" tot. charge = "<<TotCharge<<endl;
      continue;
    }
    if(verboseLevel==1){
      if(nGeaneTrks!=0 && sumID!=4424 && sumID!=0){ 
	cout<<" "<<endl;
	cout<<"sumIDs="<<sumID<<endl;
      }
    }
    if(sumID!=0 && nGeaneTrks!=0){  
    //    if(sumID!=0){  
      hMCpdgSUM->Fill(sumID);
      hMCpdgSUM_num->Fill(sumID,nParticles);
    }
    ///-----------------------------------------------------------------------------------------
    // if(sumID==4424) {
      for (Int_t iN=0; iN<nParticles; iN++){
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(iN);
	Int_t mcID = mctrk->GetPdgCode();
	TVector3 StartMC = mctrk->GetStartVertex();
	TVector3 MomMC = mctrk->GetMomentum();
	if(mcID==-2212 && sumID==4424){
	  hVxMCAP->Fill(StartMC.X());
	  hVyMCAP->Fill(StartMC.Y());
	  hVzMCAP->Fill(StartMC.Z());
	  hPMCAP->Fill(MomMC.Mag());
	  hThetaMCAP->Fill(MomMC.Theta());
	  hPhiMCAP->Fill(MomMC.Phi());
    	  thetaMC = 1000*MomMC.Theta();
	  if(MomMC.Mag()>Plab+0.05 || thetaMC<0. || thetaMC>1000.) continue;
	  cout<<"thetaMC = "<<thetaMC<<endl;
	  tThetaMC->Fill();
	}
	nmcall->Fill(mcID,sumID,StartMC.X(),StartMC.Y(),StartMC.Z(),
		     MomMC.X(),MomMC.Y(), MomMC.Z(),
		     MomMC.Mag(), MomMC.Theta(), MomMC.Phi());
      }
      //    }


    if(nGeaneTrks==0) continue;
    if(verboseLevel>2)  cout<<"Number of sim.particles: "<<nParticles<<" number of rec.trks: "<<numTrk<<" number of geane trks: "<<nGeaneTrks<<endl;
    /// Read INFO from reconstructed tracks ----------------------------------------------------
       
    for (Int_t iN=0; iN<nGeaneTrks; iN++){
      Int_t diffIDs=1;
      vector<int> MCtrk;
      vector<int> MCtrkID;
      vector<int> MCtrkIDcount;// counts how many times MCtrk with this ID participate in this RECtrk
      for(int hg=0;hg<nParticles;hg++)
	MCtrk.push_back(1);
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(iN);
      Double_t lyambda = fRes->GetLambda();
      if(lyambda==0){
	cout<<"GEANE didn't propagate this trk!"<<endl;}
      if(lyambda==0) continue;
 
	TVector3 posRec = fRes->GetPosition();
	hVxRec->Fill(posRec.X());
	hVyRec->Fill(posRec.Y());
	hVzRec->Fill(posRec.Z());
	Double_t errX = fRes->GetDX();
	Double_t errY = fRes->GetDY();
	Double_t errZ = fRes->GetDZ();

	Double_t thetaRec = TMath::Pi()/2. - lyambda;
	hThetaRec->Fill(thetaRec);
	Double_t err_lyambda = fRes->GetDLambda();

	Double_t phiRec = fRes->GetPhi();
	hPhiRec->Fill(phiRec);
	Double_t err_phi = fRes->GetDPhi();

	TVector3 MomRec = fRes->GetMomentum();
	hPRec->Fill(MomRec.Mag());
	hPxRec->Fill(MomRec.X());
	hPyRec->Fill(MomRec.Y());
	hPzRec->Fill(MomRec.Z());
	Double_t errMomRec = fRes->GetDQp();
	Double_t errPx = fRes->GetDPx();
	Double_t errPy = fRes->GetDPy();
	Double_t errPz = fRes->GetDPz();
	nrecall->Fill(posRec.X(),posRec.Y(),posRec.Z(),
		      MomRec.X(),MomRec.Y(),MomRec.Z(),
		      MomRec.Mag(),MomRec.Theta(),MomRec.Phi());
	PndLinTrack *trk_lin = (PndLinTrack*)rec_trk->At(iN);
	Int_t candID = trk_lin->GetTCandID();
	PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);
	const int Ntrkcandhits= trkcand->GetNHits();
	//	cout<<"trk #"<<iN<<": ";
	for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){
	  PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	  Int_t hitID = candhit.GetHitId();
	  PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	  
	  ///Top cluster
	  PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	  PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	  if (astripdigi->GetIndex(0) == -1) continue; // sort out noise
	  PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	  int MCidTOP = MCPoint->GetTrackID();
	  
	  ///Bottom cluster
	  Int_t  botIndex = myHit->GetBotIndex();
	  PndSdsClusterStrip* myClusterBot =  (PndSdsClusterStrip*)(fStripClusterArray->At(botIndex));
	  PndSdsDigiStrip* astripdigiBot = (PndSdsDigiStrip*)(fStripDigiArray->At(myClusterBot->GetDigiIndex(0)));
	  if (astripdigiBot->GetIndex(0) == -1) continue; // sort out noise
	  PndSdsMCPoint* MCPointBot = (PndSdsMCPoint*)(true_points->At(astripdigiBot->GetIndex(0)));
	  int MCidBOT = MCPointBot->GetTrackID();
	  //	if(MCidTOP!=MCidBOT) continue;
	  MCtrkID.push_back(MCidTOP);
	  MCtrkID.push_back(MCidBOT);
	  MCtrkIDcount.push_back(1);
	  MCtrkIDcount.push_back(1);
	  //	  cout<<" "<<MCidTOP<<" "<<MCidBOT;
	}
	//	cout<<""<<endl;
	
	Int_t k, x;
	bool ch=false; //Was element changed? 
	Int_t nch = 0; //How many times?
	for(Int_t n=0; n<MCtrkID.size(); n++) { // n - current position
	  k=n; x=MCtrkID[n];
	  for(Int_t m=n+1; m<MCtrkID.size(); m++)	// find the least element
	    if (MCtrkID[m]<x){
	      k=m; x=MCtrkID[m];	        // k - index for the least element
	      ch=true; nch++;
	    }
	  MCtrkID[k] = MCtrkID[n]; MCtrkID[n] = x; // change position between the least and current elements
	}
	bool diffMChits=false;
	if(ch) diffMChits=true;
	else{
	  for(Int_t n=0; n<MCtrkID.size(); n++) 
	    if(MCtrkID[n]!=MCtrkID[0]) diffMChits=true;
	}
	//	cout<<"  --------------  "<<endl;
	for(Int_t n=0; n<MCtrkID.size(); n++) {
	  for(Int_t m=0; m<MCtrkID.size(); m++) {
	    if(m==n) continue;
	    if(MCtrkID[n]==MCtrkID[m]){
	      MCtrkIDcount[n] = MCtrkIDcount[n]+1;
	    }
	  }
	}

	if(diffMChits){//reconstructed trk contains hits from different MC trks
	  //print information for study
	  cout<<"======================"<<endl;
	  cout<<"Number of sim.particles: "<<nParticles<<" number of rec.trks: "<<numTrk<<" number of geane trks: "<<nGeaneTrks<<endl;
	  for(Int_t n=0; n<MCtrkID.size(); n++) {
	    cout<<MCtrkID[n]<<" ";
	  }
	  cout<<""<<endl;

	 

	  Int_t prevID = MCtrkID[0];
	  diffIDs = 1;
	  bool fillRecTrk=true;
	  ///Fill MC hists --------------------------------------------------------
	  PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(MCtrkID[0]);
	  Int_t mcID = mctrk->GetPdgCode();
	  hMCpdg->Fill(mcID);
	  hMCtrkNhits->Fill(MCtrkIDcount[0], mcID);
	  double partHits = 100.*(double(MCtrkIDcount[0])/MCtrkIDcount.size());
	  if(MCtrkIDcount.size()!=8)	
	  cout<<"partHits = "<<partHits <<" MCtrkIDcount[0] = "<<MCtrkIDcount[0]<<" MCtrkIDcount.size() = "<<MCtrkIDcount.size()<<endl;
	  hMCtrkPhits->Fill(partHits, mcID);
	  cout<<"MCtrkIDcount[0] = "<<MCtrkIDcount[0]<<" mcID = "<<mcID<<endl;
	  TVector3 MomMC = mctrk->GetMomentum();
	  hPMC->Fill(MomMC.Mag());
	  hPxMC->Fill(MomMC.X());
	  hPyMC->Fill(MomMC.Y());
	  hPzMC->Fill(MomMC.Z());
	  Double_t thetaMC = MomMC.Theta();
	  hThetaMC->Fill(thetaMC);
	  Double_t phiMC = MomMC.Phi();
	  hPhiMC->Fill(phiMC);
	  TVector3 StartMC = mctrk->GetStartVertex();
	  hVxMC->Fill(StartMC.X());
	  hVyMC->Fill(StartMC.Y());
	  hVzMC->Fill(StartMC.Z());
	  ///Fill histgs for antiproton------------------------
	  if(mcID==-2212 && sumID==4424 && fillRecTrk) {
	    hVxRecAP->Fill(posRec.X());
	    hVyRecAP->Fill(posRec.Y());
	    hVzRecAP->Fill(posRec.Z());
	    hPxRecAP->Fill(MomRec.X());
	    hPyRecAP->Fill(MomRec.Y());
	    hPzRecAP->Fill(MomRec.Z());
	    hThetaRecAP->Fill(thetaRec);
	    hThetaRecAPnoise->Fill(thetaRec);
	    hPhiRecAP->Fill(phiRec);
	    hPRecAP->Fill(MomRec.Mag());
	    fillRecTrk=false;//put information from reconstructed track only once!
	  }
	  ///----------------------------------------------------------------------

	  for(Int_t n=1; n<MCtrkID.size(); n++){ 
	    if(verboseLevel>2)   cout<<MCtrkID[n]<<" =MCtrkID["<<n<<"]"<<endl;
	    if(prevID<MCtrkID[n]){
	      ///Fill MC hists --------------------------------------------------------
	      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(MCtrkID[n]);
	      Int_t mcID = mctrk->GetPdgCode();
	      hMCpdg->Fill(mcID);
	      hMCtrkNhits->Fill(MCtrkIDcount[n], mcID);
	      double partHits = 100.*(double(MCtrkIDcount[n])/MCtrkIDcount.size());
	      if(MCtrkIDcount.size()!=8)	
	      cout<<"partHits = "<<partHits
		  <<" MCtrkIDcount[n] = "<<MCtrkIDcount[n]<<" MCtrkIDcount.size() = "<<MCtrkIDcount.size()<<endl;
	      hMCtrkPhits->Fill(partHits, mcID);
	      TVector3 MomMC = mctrk->GetMomentum();
	      hPMC->Fill(MomMC.Mag());
	      hPxMC->Fill(MomMC.X());
	      hPyMC->Fill(MomMC.Y());
	      hPzMC->Fill(MomMC.Z());
	      Double_t thetaMC = MomMC.Theta();
	      hThetaMC->Fill(thetaMC);
	      Double_t phiMC = MomMC.Phi();
	      hPhiMC->Fill(phiMC);
	      TVector3 StartMC = mctrk->GetStartVertex();
	      hVxMC->Fill(StartMC.X());
	      hVyMC->Fill(StartMC.Y());
	      hVzMC->Fill(StartMC.Z());
	      ///Fill histgs for antiproton------------------------
	      if(mcID==-2212 && sumID==4424 && fillRecTrk) {
       		hVxRecAP->Fill(posRec.X());
		hVyRecAP->Fill(posRec.Y());
		hVzRecAP->Fill(posRec.Z());
		hPxRecAP->Fill(MomRec.X());
		hPyRecAP->Fill(MomRec.Y());
		hPzRecAP->Fill(MomRec.Z());
		hThetaRecAP->Fill(thetaRec);
		hThetaRecAPnoise->Fill(thetaRec);
		hPhiRecAP->Fill(phiRec);
		hPRecAP->Fill(MomRec.Mag());
		nrecsig->Fill(mcID,posRec.X(),posRec.Y(),posRec.Z(),
			      MomRec.X(),MomRec.Y(),MomRec.Z(),
			      MomRec.Mag(),MomRec.Theta(),MomRec.Phi());
		fillRecTrk=false;//put information from reconstructed track only once!
	      }
	      else{
		nrecbkg->Fill(mcID,sumID,posRec.X(),posRec.Y(),posRec.Z(),
			      MomRec.X(),MomRec.Y(),MomRec.Z(),
			      MomRec.Mag(),MomRec.Theta(),MomRec.Phi());
	      }
	      ///----------------------------------------------------------------------
	      diffIDs++;
	      prevID=MCtrkID[n];
	      for(int nk=0;nk<numTrk;nk++){
		if(MCtrkID[n]==nk)
		  MCtrk[nk]++;
	      }
	    }
	  }
	  if(verboseLevel>2) {
	    cout<<"Trk #"<<iN<<" diffIDs = "<<diffIDs<<endl;
	    cout<<"MCtrk["<<iN<<"]="<<MCtrk[iN]<<endl;
	  }
	}
	else{//reconstructed trk contains hits from one MC trks
	    ///Fill MC histgs --------------------------------------------------------
	    PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(MCtrkID[0]);
	    Int_t mcID = mctrk->GetPdgCode();
	    hMCpdg->Fill(mcID);
	    hMCtrkNhits->Fill(MCtrkIDcount[0], mcID);
	    double partHits = 100.*(double(MCtrkIDcount[0])/MCtrkIDcount.size());
	    if(MCtrkIDcount.size()!=8)	   
	      cout<<"partHits = "<<partHits <<" MCtrkIDcount[0] = "
		  <<MCtrkIDcount[0]<<" MCtrkIDcount.size() = "<<MCtrkIDcount.size()<<endl;
	    hMCtrkPhits->Fill(partHits, mcID);
	    TVector3 MomMC = mctrk->GetMomentum();
	    hPMC->Fill(MomMC.Mag());
	    hPxMC->Fill(MomMC.X());
	    hPyMC->Fill(MomMC.Y());
	    hPzMC->Fill(MomMC.Z());
	    Double_t thetaMC = MomMC.Theta();
	    hThetaMC->Fill(thetaMC);
	    Double_t phiMC = MomMC.Phi();
	    hPhiMC->Fill(phiMC);
	    TVector3 StartMC = mctrk->GetStartVertex();
	    hVxMC->Fill(StartMC.X());
	    hVyMC->Fill(StartMC.Y());
	    hVzMC->Fill(StartMC.Z());
	    ///Fill histgs for antiproton from el.scattering ---
	    if(mcID==-2212 && sumID==4424) {
	      hVxRecAP->Fill(posRec.X());
	      hVyRecAP->Fill(posRec.Y());
	      hVzRecAP->Fill(posRec.Z());
	      hPxRecAP->Fill(MomRec.X());
	      hPyRecAP->Fill(MomRec.Y());
	      hPzRecAP->Fill(MomRec.Z());
	      hThetaRecAP->Fill(thetaRec);
	      hPhiRecAP->Fill(phiRec);
	      hPRecAP->Fill(MomRec.Mag());
	      nrecsig->Fill(mcID,posRec.X(),posRec.Y(),posRec.Z(),
			    MomRec.X(),MomRec.Y(),MomRec.Z(),
			    MomRec.Mag(),MomRec.Theta(),MomRec.Phi());
	    }
	    else{
	      nrecbkg->Fill(mcID,sumID,posRec.X(),posRec.Y(),posRec.Z(),
			      MomRec.X(),MomRec.Y(),MomRec.Z(),
			      MomRec.Mag(),MomRec.Theta(),MomRec.Phi());
	    }
	  ///----------------------------------------------------------
	}
	hMCtrk->Fill(diffIDs);
    }
    ///-----------------------------------------------------------------------------------------
  }

  if(fillBkgTree){ //read track after back-propagation with GEANE
    tTheta = new TTree("tThetaRec", "#theta_{rec}");
    Double_t thetaRecBkg;
    Double_t thetaRec;
    tTheta->Branch("thetaRecBkg", &thetaRecBkg);
    tTheta->Branch("thetaRec", &thetaRec);
    for (Int_t j=0; j<nEvents; j++)
      {
	// Read GEANE -----------------------------------------------------------------
	tgeane.GetEntry(j);
	//	last = DrawProgressBar(50, (j+1)/((double)nEvents),last);
	const int nGeaneTrks = geaneArray->GetEntriesFast();
	
	for (Int_t i=0; i<nGeaneTrks; i++){
	  FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(i);
	  if(!fRes) { cout << "no fRes!" << endl; continue;}
	  Double_t lyambda = fRes->GetLambda();
	  thetaRec = (TMath::Pi()/2. - lyambda)*1000.;
	  TVector3 posRec = fRes->GetPosition();
	  TVector3 MomRec = fRes->GetMomentum();
	  if(MomRec.Mag()>Plab+0.05 ||  thetaRec<2.9 ||  thetaRec>8.1) continue;
	  thetaRecBkg = -10.;
	  //  if(fabs(posRec.X()*posRec.X()+posRec.Y()*posRec.Y())>0.5){
	  if(fabs(posRec.X())>1.){
	    thetaRecBkg = thetaRec;
	    //  branchBkg->Fill();
	  }
	  tTheta->Fill();
	}//geane tracks
	//----------------------------------------------------------------------------------
	
      }//events
    tTheta->Write("", TObject::kOverwrite);

    // tThetaMC = new TTree("tThetaMC", "#theta_{MC}");
    // Double_t thetaMC;
    // tThetaMC->Branch("thetaMC", &thetaMC);
    // for (Int_t j=0; j<nEvents; j++)
    //   {
    // 	// Read MC -----------------------------------------------------------------
    // 	tMC.GetEntry(j);
    // 	const int nParticles = true_tracks->GetEntriesFast();
    // 	for (Int_t iN=0; iN<nParticles; iN++){
    // 	  PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(iN);
    // 	  Int_t mcID = mctrk->GetPdgCode();
    // 	  if(mcID!=-2212) continue;    
    // 	  TVector3 MomMC_all = mctrk->GetMomentum();
    // 	  thetaMC = 1000*MomMC_all.Theta();
    // 	  if(MomMC_all.Mag()>Plab+0.05 || thetaMC<0. || thetaMC>1000.) continue;
    // 	  cout<<"thetaMC = "<<thetaMC<<endl;
    // 	  tThetaMC->Fill();
    // 	}//MC tracks
    // 	//----------------------------------------------------------------------------------
    //   }//events
    // tThetaMC->Write("", TObject::kOverwrite);
  }

  hMCtrkNhits->Write();
  hMCtrkPhits->Write();
  nrecsig->Write();
  nrecbkg->Write();
  nrecall->Write();
  nmcall->Write();
  hVxRec->Write(); 
  hVyRec->Write(); 
  hVzRec->Write(); 
  hVxRecAP->Write(); 
  hVyRecAP->Write(); 
  hVzRecAP->Write(); 
  hVxMC_all->Write(); 
  hVyMC_all->Write();
  hVzMC_all->Write(); 
  hVxMC->Write(); 
  hVyMC->Write();
  hVzMC->Write(); 
  hVxMCAP->Write(); 
  hVyMCAP->Write();
  hVzMCAP->Write(); 
  hPxRec->Write(); 
  hPyRec->Write(); 
  hPzRec->Write(); 
  hPxRecAP->Write(); 
  hPyRecAP->Write(); 
  hPzRecAP->Write(); 
  hPxMC_all->Write(); 
  hPyMC_all->Write(); 
  hPzMC_all->Write(); 
  hPxMC->Write(); 
  hPyMC->Write(); 
  hPzMC->Write(); 
  
  hPRec->Write(); 
  hPhiRec->Write(); 
  hThetaRec->Write(); 
  hPRecAP->Write(); 
  hPhiRecAP->Write(); 
  hThetaRecAP->Write(); 
  hThetaRecAPnoise->Write();

  hPMC_all->Write(); 
  hThetaMC_all->Write(); 
  hPhiMC_all->Write(); 
  hMCpdg_all->Write();

  hPMC->Write(); 
  hThetaMC->Write(); 
  hPhiMC->Write(); 
  hPMCAP->Write(); 
  hThetaMCAP->Write(); 
  hPhiMCAP->Write(); 
  hMCpdg->Write();

  hMCtrk->Write();
  hMCpdgSUM->Write();
  TCanvas *c1 = new TCanvas("MC_all_mom");
  c1->Divide(3,2);
  c1->cd(1);
  hPxMC_all->Draw();
  c1->cd(2);
  hPyMC_all->Draw();
  c1->cd(3);
  hPzMC_all->Draw();
  c1->cd(4);
  hPMC_all->Draw();
  c1->cd(5);
  hPhiMC_all->Draw();
  c1->cd(6);
  hThetaMC_all->Draw();
  c1->Write();

  TCanvas *c2 = new TCanvas("Rec_mom");
  c2->Divide(3,2);
  c2->cd(1);
  hPxRec->Draw();
  hPxMC->SetLineColor(2);
  hPxMC->Draw("same");
  hPxRecAP->SetLineColor(3);
  hPxRecAP->Draw("same");
  c2->cd(2);
  hPyRec->Draw();
  hPyMC->SetLineColor(2);
  hPyMC->Draw("same");
  hPyRecAP->SetLineColor(3);
  hPyRecAP->Draw("same");
  c2->cd(3);
  hPzRec->Draw();
  hPzMC->SetLineColor(2);
  hPzMC->Draw("same");
  hPzRecAP->SetLineColor(3);
  hPzRecAP->Draw("same");
  c2->cd(4);
  hPRec->Draw();
  hPMC->SetLineColor(2);
  hPMC->Draw("same");
  hPRecAP->SetLineColor(3);
  hPRecAP->Draw("same");
  c2->cd(5);
  hPhiRec->Draw();
  hPhiMC->SetLineColor(2);
  hPhiMC->Draw("same");
  hPhiRecAP->SetLineColor(3);
  hPhiRecAP->Draw("same");
  c2->cd(6);
  hThetaRec->Draw();
  hThetaMC->SetLineColor(2);
  hThetaMC->Draw("same");
  hThetaRecAP->SetLineColor(3);
  hThetaRecAP->Draw("same");
  c2->Write();

 TCanvas *c3 = new TCanvas("init_vtx");
 c3->Divide(3,2);
 c3->cd(1);
 hVxMC_all->Draw();
 c3->cd(2);
 hVyMC_all->Draw();
 c3->cd(3);
 hVzMC_all->Draw();
 c3->cd(4);
 hVxRec->Draw();
 hVxMC->SetLineColor(2);
 hVxMC->Draw("same");
 hVxRecAP->SetLineColor(3);
 hVxRecAP->Draw("same");
 c3->cd(5);
 hVyRec->Draw();
 hVyMC->SetLineColor(2);
 hVyMC->Draw("same");
 hVyRecAP->SetLineColor(3);
 hVyRecAP->Draw("same");
 c3->cd(6);
 hVzRec->Draw();
 hVzMC->SetLineColor(2);
 hVzMC->Draw("same");
 hVzRecAP->SetLineColor(3);
 hVzRecAP->Draw("same");
 c3->Write();
 
 TCanvas *c4 = new TCanvas("ID");
 c4->Divide(2,2);
 c4->cd(1);
 hMCpdg_all->Draw();
 c4->cd(2);
 hMCpdg->SetLineColor(2);
 hMCpdg->Draw();
 c4->cd(3);
 hMCtrk->Draw();
 c4->Write();

 tThetaMC->Write("", TObject::kOverwrite);
 hMCpdgSUM_num->Write();
 hRecNtrk->Write();
 f->Close();
 if(verboseLevel>0)  cout<<"Macro succsefully finished!"<<endl;
}
