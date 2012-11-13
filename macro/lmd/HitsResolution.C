//################################################################
//# Macros for studing hits resolution
//# author: Anastasia Karavdina
//# date: November, 2012
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(hits_study HitsResolution.C)
//#target_link_libraries(hits_study ${ROOT_LIBRARIES} genfit genfitAdapters trackrep RecoHits Stt Fts Lmd LmdTrk Sds SdsReco Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase MathMore Minuit)
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./hits_study --help
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
#include<PndSdsMCPoint.h>

//lmd track
#include<PndLinTrack.h>
#include<PndTrackCand.h>
#include<PndTrackCandHit.h>
#include<PndSdsHit.h>
#include<PndSdsClusterPixel.h>
#include<PndSdsDigiPixel.h>
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

// needed for corditate from\out lumi frame calculation
#include<PndLmdDim.h>

using namespace std;

int main(int __argc,char *__argv[]) {
  ofstream output;
  int nEvents=1000;
  int startEvent=0;
  TString storePath="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutput";
  double Plab=15.;
  int verboseLevel=0;
  int sentype=0;
  std::string startStr="", momStr="", nStr="", pathStr="", verbStr="", outrootStr="", sensorTypeStr="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    
    std::cout << "This is script for study hits residuals for pixel sensors design \n"
	      << "with parameters\n"
	      <<"-s start event \n"
	      <<"-n Number of events \n"
	      <<"-path path to the file(s) \n"
	      <<"-out name for output file with ntuples \n"
	      <<"-st sensor type: 0=strip 1=pixel"
	      <<"-v verbose Level (if>0, print out some information) \n"
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
    if (sw=="-st"){
      optind++;
      sensorTypeStr = __argv[optind];
      found=true;
    }
    if(sw=="-out"){
      optind++;
      outrootStr= __argv[optind];
      found=true;
    }
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
  }

  std::stringstream startSStr(startStr), nSStr(nStr), pathSStr(pathStr), verbSStr(verbStr), sensorTypeSStr(sensorTypeStr);

  startSStr >> startEvent; 
  nSStr >> nEvents;
  pathSStr >> storePath;
  verbSStr >> verboseLevel;
  sensorTypeSStr >> sentype;
  cout<<"For data files will be used path: "<<storePath<<endl;

  // ---- Open output file ------------------------------------------------
  TFile *f = new TFile(outrootStr.c_str(),"RECREATE");
   // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // ---- Input files --------------------------------------------------------
 
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

  TString simMC=storePath+"/Lumi_MC_";
  simMC += startEvent;
  simMC += ".root";
  TChain tMC("cbmsim");
  tMC.Add(simMC);
  
  // ---------------------------------------------------------------------------------

  TString collNameDigis;
  TString collNameCluster;
  TString typArrayCluster;
  TString typArrayDigi;
  TString collNameHits;
  if(sentype>0){
    collNameDigis="LMDPixelDigis";
    collNameCluster="LMDPixelClusterCand";
    typArrayCluster = "PndSdsClusterPixel";
    typArrayDigi = "PndSdsDigiPixel";
    collNameHits = "LMDHitsPixel";
  }
  else{
    collNameDigis = "LMDStripDigis";
    collNameCluster = "LMDStripClusterCand";
    typArrayCluster = "PndSdsClusterStrip";
    typArrayDigi = "PndSdsDigiStrip";
    collNameHits = "LMDHitsStrip";
  }
  
  //--- Digitization info ------------------------------------------------------------
  TClonesArray* fStripClusterArray;
  fStripClusterArray = new TClonesArray(typArrayCluster);
  tHits.SetBranchAddress(collNameCluster,&fStripClusterArray);
  TClonesArray* fStripDigiArray;
  fStripDigiArray = new TClonesArray(typArrayDigi);
  tdigiHits.SetBranchAddress(collNameDigis,&fStripDigiArray);
  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  tHits.SetBranchAddress(collNameHits,&rechit_array);  //Points for Tracks
  //----------------------------------------------------------------------------------
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  //----------------------------------------------------------------------------------  
  
  //--- MC info -----------------------------------------------------------------
  TClonesArray* true_tracks=new TClonesArray("PndMCTrack");
  tMC.SetBranchAddress("MCTrack",&true_tracks);  //True Track to compare
  
  TClonesArray* true_points=new TClonesArray("PndSdsMCPoint");
  tMC.SetBranchAddress("LMDPoint",&true_points);  //True Points to compare
  //----------------------------------------------------------------------------------

  //Histograms with residuals---------------------------------------------------------
  TNtuple *nhits = new TNtuple("nhits","rec hits compare to mc hits","xrec:yrec:zrec:errxrec:erryrec:errzrec:xmc:ymc:zmc:sens_id:plane:module:side:phi:theta");
  TNtuple *nhitsMCtrks = new TNtuple("nhitsMCtrks","rec hits compare to mc trks","xrec:yrec:zrec:errxrec:erryrec:errzrec:xmcTrk:ymcTrk:zmcTrk:sens_id:plane:module:side:phi:theta");

  TNtuple *nMConly = new TNtuple("nMConly","mc hits compare to mc trks","mcmom:xmc:ymc:zmc:xmcTrk:ymcTrk:zmcTrk:xDr:yDr:zDr:sens_id:half:plane:module:side");
  //Load lumi geo params
  PndLmdDim *lmddim = PndLmdDim::Instance();
  // lmddim -> Read_transformation_matrices("matrices.txt", true);
  lmddim -> Read_transformation_matrices("matrices_perfect.txt", false);

  for (Int_t j=0; j<nEvents; j++){
    // Read REC tree -----------------------------------------------------------------
    tTrkCand.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);
    tMC.GetEntry(j);
    ///-----------------------------------------------------------------------------------------

    const int nRecTrks = rec_trk->GetEntriesFast();


    /// Read info about hits from reconstructed tracks ----------------------------------------------------
    TVector3 startPosMCtrk;
    TVector3 startDirMCtrk;

    for (Int_t iN=0; iN<nRecTrks; iN++){
      PndLinTrack *trk_lin = (PndLinTrack*)rec_trk->At(iN);
      TVector3 startlintrk = trk_lin->GetStartVec();
      TVector3 dirlintrk = trk_lin->GetDirectionVec();
      Int_t candID = trk_lin->GetTCandID();
      PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);
      const int Ntrkcandhits= trkcand->GetNHits();
      if(Ntrkcandhits<4) continue; //!!! TEST with 4 hits tracks only !!!
      double phiMCgl;
      TVector3 startPosMC, startDirMC;
	for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){
	  TVector3 mcDir;
	  PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	  Int_t hitID = candhit.GetHitId();
	  PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	  TVector3 HitPos = myHit->GetPosition(); 
	  TMatrixD HitCov = myHit->GetCov(); 
	  TVector3 HitPosErr(sqrt(HitCov(0,0)),sqrt(HitCov(1,1)),sqrt(HitCov(2,2)));
	  TVector3 mcMid,mcTop,mcTopOUT, mcMom;
	  TVector3 mcTrkPointREC,mcTrkPointMC;
	  int sensorID = myHit->GetSensorID();
	  int ihalf, iplane, imodule, iside, idie, isensor;
	  if(sentype>0){//pixel
	    PndSdsClusterPixel* myCluster = (PndSdsClusterPixel*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	    PndSdsDigiPixel* astripdigi = (PndSdsDigiPixel*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	    PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	    mcTop = MCPoint->GetPosition(); 
	    mcTopOUT = MCPoint->GetPositionOut();
	    // mcTop = lmddim->Transform_global_to_lmd_local(mcTop, false, false);
	    // mcTopOUT = lmddim->Transform_global_to_lmd_local(mcTopOUT, false, false);
	    double xMom = MCPoint->GetPx();
	    double yMom = MCPoint->GetPy();
	    double zMom = MCPoint->GetPz();
	    mcMom.SetXYZ(xMom,yMom,zMom);
	    // calculate the plane and sensor on this plane
	    lmddim->Get_sensor_by_id(sensorID, ihalf, iplane, imodule, iside, idie, isensor);
	  }
	  else{//strip
	    PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	    PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	    if (astripdigi->GetIndex(0) == -1) continue; // sort out noise
	    PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	    mcTop = MCPoint->GetPosition(); 
	    mcTopOUT = MCPoint->GetPositionOut();
	    double xMom = MCPoint->GetPx();
	    double yMom = MCPoint->GetPy();
	    double zMom = MCPoint->GetPz();
	    mcMom.SetXYZ(xMom,yMom,zMom);
	    iplane = int(sensorID/8);
	    iside = -1;
	    idie = -1;
	    ihalf= -1;
	    imodule= -1;
	    isensor = sensorID%8;
	  }
	 
	  // mcMid = mcTop+(mcTopOUT-mcTop)*0.5;
	  mcMid = mcTop; //TEST
	  double MCmom = mcMom.Mag();
	  double inv_MCmom = 1./MCmom;
	  mcDir = inv_MCmom*mcMom;
	  if(iHit==0){
	    startPosMC = mcMid;
	    startDirMC = mcDir;
	    if(sentype==0){ //for strip sensors rec hit difinded in the middle, but MC on the surface
	      double sh_z = 0.0075;
	      double sh_x = sh_z*startDirMC.X();
	      double sh_y = sh_z*startDirMC.Y();
	      TVector3 sh_v(sh_x,sh_y,sh_z);
	      startPosMC +=sh_v;
	    }
	    mcTrkPointMC = startPosMC;
	    mcTrkPointREC = startPosMC;//??? is it correct???
	  }
	  else{
	    // if(iHit==1){
	    //   startDirMC = mcMid-startPosMC;
	    //   startDirMC *=(1./startDirMC.Mag());
	    //   mcTrkPointMC = mcMid;
	    //   double xnew = startPosMC.X()+startDirMC.X()*(HitPos.Z()-startPosMC.Z());
	    //   double ynew = startPosMC.Y()+startDirMC.Y()*(HitPos.Z()-startPosMC.Z());
	    //   double znew = HitPos.Z();
	    //   mcTrkPointREC.SetXYZ(xnew,ynew,znew);
	    // }
	    // else{
	    // cout<<"startDirMC: "<<endl;
	    // startDirMC.Print();
	      double xnew = startPosMC.X()+startDirMC.X()*(HitPos.Z()-startPosMC.Z());
	      double ynew = startPosMC.Y()+startDirMC.Y()*(HitPos.Z()-startPosMC.Z());
	      double znew = HitPos.Z();
	      mcTrkPointREC.SetXYZ(xnew,ynew,znew);
	      
	      double xnewmc = startPosMC.X()+startDirMC.X()*(mcMid.Z()-startPosMC.Z());
	      double ynewmc = startPosMC.Y()+startDirMC.Y()*(mcMid.Z()-startPosMC.Z());
	      double znewmc = mcMid.Z();
	      mcTrkPointMC.SetXYZ(xnewmc,ynewmc,znewmc);
	      // }
	  }
	
	 
	  
	  nhits->Fill(HitPos.X(),HitPos.Y(),HitPos.Z(),HitPosErr.X(),HitPosErr.Y(),HitPosErr.Z(),mcMid.X(),mcMid.Y(),mcMid.Z(),sensorID,iplane,imodule,iside,startDirMC.Phi(),startDirMC.Theta());
	  nhitsMCtrks->Fill(HitPos.X(),HitPos.Y(),HitPos.Z(),HitPosErr.X(),HitPosErr.Y(),HitPosErr.Z(),mcTrkPointREC.X(),mcTrkPointREC.Y(),mcTrkPointREC.Z(),sensorID,iplane,imodule,iside,startDirMC.Phi(),startDirMC.Theta());
	  nMConly->Fill(MCmom,mcMid.X(),mcMid.Y(),mcMid.Z(),mcTrkPointMC.X(),mcTrkPointMC.Y(),mcTrkPointMC.Z(),mcDir.X(),mcDir.Y(),mcDir.Z(),sensorID,ihalf,iplane,imodule,iside);
	}
    }/// end rec-trks
    ///-----------------------------------------------------------------------------------------
  }/// end events
  nhits->Write();
  nhitsMCtrks->Write();
  nMConly->Write();
  f->Close();
  
}
