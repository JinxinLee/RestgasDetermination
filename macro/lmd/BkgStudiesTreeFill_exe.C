//################################################################
//# Macros for Background\Signal study
//# !need all files from full reconstruction chain (results from GEANE, MC etc.)
//# author: Anastasia Karavdina
//# date: April, 2012
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(bkg_tree_fill BkgStudiesTreeFill_exe.C)
//# target_link_libraries(bkg_tree_fill ${ROOT_LIBRARIES} genfit genfitAdapters trackrep RecoHits Stt Fts Lmd LmdTrk LmdReco Sds SdsReco Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase MathMore Minuit)
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./bkg_tree_fill --help
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
#include<PndSdsClusterPixel.h>
#include<PndSdsDigiPixel.h>
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
  
  //strip
  // TString recHit=storePath+"/Lumi_reco_";
  // recHit += startEvent;
  // recHit += ".root";
  // TChain tHits("cbmsim");
  // tHits.Add(recHit);

  //pixel
 TString recHit=storePath+"/Lumi_reco_";
 recHit += startEvent;
 recHit += ".root";
 TChain tHits("cbmsim");
 tHits.Add(recHit);

 TString recHitmerged=storePath+"/Lumi_recoMerged_";
 recHitmerged += startEvent;
 recHitmerged += ".root";
 TChain tHitsMerged("cbmsim");
  tHitsMerged.Add(recHitmerged);

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
  // TClonesArray* fStripClusterArray = new TClonesArray("PndSdsClusterStrip");
  // tHits.SetBranchAddress("LMDStripClusterCand",&fStripClusterArray); 
  
  // TClonesArray* fStripDigiArray = new TClonesArray("PndSdsDigiStrip");
  // tdigiHits.SetBranchAddress("LMDStripDigis",&fStripDigiArray); 

   TClonesArray* fStripClusterArray = new TClonesArray("PndSdsClusterPixel");
   tHits.SetBranchAddress("LMDPixelClusterCand",&fStripClusterArray);
   TClonesArray* fStripDigiArray = new TClonesArray("PndSdsDigiPixel");
   tdigiHits.SetBranchAddress("LMDPixelDigis",&fStripDigiArray);
 
  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
   //  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  //  tHits.SetBranchAddress("LMDHitsStrip",&rechit_array);  //Points for Tracks

   TClonesArray* rechit_array = new TClonesArray("PndSdsMergedHit");
   tHitsMerged.SetBranchAddress("LMDHitsMerged",&rechit_array);  //Points for Tracks
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
  
  
  TTree *tAll = new TTree("tAll","Reconstructed variables (sig+bkg)");
  TTree *tSig = new TTree("tSig","Reconstructed variables (sig)");
  TTree *tBkg= new TTree("tBkg","Reconstructed variables (bkg)");
  Double_t glTheta,glPhi,glXpca,glYpca,glZpca,glPx,glPy,glPz,glP;
  Double_t glerrTheta,glerrPhi,glerrXpca,glerrYpca,glerrZpca,glerrPx,glerrPy,glerrPz,glerrP;
  Double_t glXmc,glYmc,glZmc,glPmc,glThetamc,glPhimc;
  Int_t glID,glSumID,glMotherID;
  Int_t glnrectrks,glnrechits;
  Double_t glChi2;
  tAll->Branch("theta",&glTheta);
  tAll->Branch("phi",&glPhi);
  tAll->Branch("x",&glXpca);
  tAll->Branch("y",&glYpca);
  tAll->Branch("z",&glZpca);
  tAll->Branch("p",&glP);
  tAll->Branch("px",&glPx);
  tAll->Branch("py",&glPy);
  tAll->Branch("pz",&glPz);
  tAll->Branch("errtheta",&glerrTheta);
  tAll->Branch("errphi",&glerrPhi);
  tAll->Branch("errx",&glerrXpca);
  tAll->Branch("erry",&glerrYpca);
  tAll->Branch("errz",&glerrZpca);
  tAll->Branch("errpx",&glerrPx);
  tAll->Branch("errpy",&glerrPy);
  tAll->Branch("errpz",&glerrPz);
  tAll->Branch("errp",&glerrP);
  tAll->Branch("xmc",&glXmc);
  tAll->Branch("ymc",&glYmc);
  tAll->Branch("zmc",&glZmc);
  tAll->Branch("pmc",&glPmc);
  tAll->Branch("thetamc",&glThetamc);
  tAll->Branch("phimc",&glPhimc);
  tAll->Branch("id",&glID);
  tAll->Branch("sumid",&glSumID);
  tAll->Branch("nrechits",&glnrechits);
  tAll->Branch("nrectrks",&glnrectrks);
  tAll->Branch("motherid",&glMotherID);
  tAll->Branch("chi2",&glChi2);
 
  Double_t glThetaSig,glPhiSig,glXpcaSig,glYpcaSig,glZpcaSig,glPxSig,glPySig,glPzSig,glPSig;
  //  Double_t glerrThetaSig,glerrPhiSig,glerrXpcaSig,glerrYpcaSig,glerrZpcaSig,glerrPxSig,glerrPySig,glerrPzSig,glerrPSig;
  tSig->Branch("theta",&glThetaSig);
  tSig->Branch("phi",&glPhiSig);
  tSig->Branch("x",&glXpcaSig);
  tSig->Branch("y",&glYpcaSig);
  tSig->Branch("z",&glZpcaSig);
  tSig->Branch("p",&glPSig);
  tSig->Branch("px",&glPxSig);
  tSig->Branch("py",&glPySig);
  tSig->Branch("pz",&glPzSig);
  // tSig->Branch("errtheta",&glerrThetaSig);
  // tSig->Branch("errphi",&glerrPhiSig);
  // tSig->Branch("errx",&glerrXpcaSig);
  // tSig->Branch("erry",&glerrYpcaSig);
  // tSig->Branch("errz",&glerrZpcaSig);
  // tSig->Branch("errpx",&glerrPxSig);
  // tSig->Branch("errpy",&glerrPySig);
  // tSig->Branch("errpz",&glerrPzSig);
  // tSig->Branch("errp",&glerrPSig);

  Double_t glThetaBkg,glPhiBkg,glXpcaBkg,glYpcaBkg,glZpcaBkg,glPxBkg,glPyBkg,glPzBkg,glPBkg;
  //Double_t glerrThetaBkg,glerrPhiBkg,glerrXpcaBkg,glerrYpcaBkg,glerrZpcaBkg,glerrPxBkg,glerrPyBkg,glerrPzBkg,glerrPBkg;
  Int_t glIDBkg,glSumIDBkg,glMotherIDBkg;
  tBkg->Branch("theta",&glThetaBkg);
  tBkg->Branch("phi",&glPhiBkg);
  tBkg->Branch("x",&glXpcaBkg);
  tBkg->Branch("y",&glYpcaBkg);
  tBkg->Branch("z",&glZpcaBkg);
  tBkg->Branch("p",&glPBkg);
  tBkg->Branch("px",&glPxBkg);
  tBkg->Branch("py",&glPyBkg);
  tBkg->Branch("pz",&glPzBkg);
  tBkg->Branch("id",&glIDBkg);
  tBkg->Branch("sumid",&glSumIDBkg);
  tBkg->Branch("motherid",&glMotherIDBkg);
  // tBkg->Branch("errtheta",&glerrThetaBkg);
  // tBkg->Branch("errphi",&glerrPhiBkg);
  // tBkg->Branch("errx",&glerrXpcaBkg);
  // tBkg->Branch("erry",&glerrYpcaBkg);
  // tBkg->Branch("errz",&glerrZpcaBkg);
  // tBkg->Branch("errpx",&glerrPxBkg);
  // tBkg->Branch("errpy",&glerrPyBkg);
  // tBkg->Branch("errpz",&glerrPzBkg);
  // tBkg->Branch("errp",&glerrPBkg);

  TNtuple *nmcall = new TNtuple("nmcall","mcAll","pdgid:sumpdgid:trkid:nparts:x:y:z:px:py:pz:p:theta:phi:nRecHitsEvent:totCharge");
  // TNtuple *nsdshits = new TNtuple("nsdshits ","sds_hits","sensid:trkid:motherID:x:y:z:charge:p:eloss:time");

  TTree *nsdshits= new TTree("nsdshits","variables for reconstructed hits");
  int hsensid,htrkid,hmotherID,hcharge,hpdgid;
  double hx,hy,hz,hp,heloss,htime;
  nsdshits->Branch("sensid",&hsensid);
 nsdshits->Branch("trkid",&htrkid);
 nsdshits->Branch("motherID",&hmotherID);
 nsdshits->Branch("charge",&hcharge);
 nsdshits->Branch("x",&hx);
 nsdshits->Branch("y",&hy);
 nsdshits->Branch("z",&hz);
 nsdshits->Branch("p",&hp);
 nsdshits->Branch("eloss",&heloss);
nsdshits->Branch("time",&htime);
 nsdshits->Branch("pdgid",&hpdgid);
  TDatabasePDG *fdbPDG = TDatabasePDG::Instance();

  for (Int_t j=0; j<nEvents; j++){
    double TotCharge = 0;
    if(verboseLevel>2)  cout<<"Event #"<<j<<endl;

    // Read GEANE & MC tree -----------------------------------------------------------------
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
    tTrkCand.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);
    tHitsMerged.GetEntry(j);
    tdigiHits.GetEntry(j);

    const int nGeaneTrks = geaneArray->GetEntriesFast();
    const int nParticles = true_tracks->GetEntriesFast();
    const int numTrk = rec_trk->GetEntriesFast();
    const int nRecHits = rechit_array->GetEntriesFast();
    if(verboseLevel>2)  cout<<"Number of sim.particles: "<<nParticles<<" number of rec.trks: "<<numTrk<<" number of geane trks: "<<nGeaneTrks<<endl;
  
    // --------------------------------------------------------------------------------------
  
    // Read Sds MC hit info -------------------------------------------------------------
    if(verboseLevel>5)  cout<<"== Read Sds MC hit info =="<<endl;
    const int nMChits = true_points->GetEntriesFast();
    if(verboseLevel>2)  cout<<"Number of sim.points: "<<nMChits<<endl;
    for(Int_t iNh=0;iNh<nMChits;iNh++){
      PndSdsMCPoint *mcHit = (PndSdsMCPoint *)true_points->At(iNh);
      int sensID = mcHit->GetSensorID();
      if(verboseLevel>5)  cout<<" sensID ="<<sensID<<endl;
      double x = mcHit->GetX();
      double y = mcHit->GetY();
      double z = mcHit->GetZ();
      if(verboseLevel>5)  cout<<" x ="<<x<<" y ="<<y<<" z ="<<z<<endl;
      double eloss = mcHit->GetEnergyLoss();
      if(verboseLevel>5)  cout<<"eloss = "<<eloss<<endl;
      double timest = mcHit->GetTime();
      if(verboseLevel>5)  cout<<"timest  = "<<timest<<endl;

      double mom = sqrt((mcHit->GetPx())*(mcHit->GetPx())+(mcHit->GetPy())*(mcHit->GetPy())+(mcHit->GetPz())*(mcHit->GetPz()));
      int trkID = mcHit->GetTrackID();
      if(verboseLevel>5)  cout<<" for MC hit#"<<iNh<<" trkID = "<< trkID<<" (trk Size = "<<nParticles<<")"<<endl;
      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(trkID);
      Int_t mcID = mctrk->GetPdgCode();
      if(verboseLevel>5)  cout<<"mcID = "<<mcID<<endl;
      if(fabs(mcID)>1e5) continue;
      TParticlePDG *fParticle = fdbPDG->GetParticle(mcID);
      Double_t  charge = fParticle->Charge();
      int motherid = mctrk->GetMotherID();
      hpdgid = mcID;
      hsensid = sensID;
      htrkid = trkID;
      hmotherID = motherid;
      hx = x;
      hy = y;
      hz = z;
      hcharge = charge;
      hp = mom;
      heloss  = eloss;
      htime = timest;
      nsdshits->Fill();
    }
    // Read Sds MC hit info (END) ----------------------------------------------------

    /// Read MC track parameters ---------------------------------------------------------------
    if(verboseLevel>5)  cout<<"== Read MC track parameters =="<<endl;
    int sumID=0;
    if(verboseLevel==1){
      cout<<"*** MC particles: "<<endl;
    }
    for (Int_t iN=0; iN<nParticles; iN++){
      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(iN);
      Int_t mcID = mctrk->GetPdgCode();
      if(fabs(mcID)>1e5) continue;
      // if(verboseLevel==1){
      // 	if(nGeaneTrks!=0){
      // 	  if(nParticles==2 && fabs(mcID)==2212){}
      // 	  else cout<<" "<<mcID;
      // 	}
      // }
      int motherid = mctrk->GetMotherID();
      if(motherid<0 && fabs(mcID)<1e5){
	sumID+=fabs(mcID);
	if(verboseLevel==1) cout<<" "<<mcID;
      }
      //   if(verboseLevel>2)   cout<<" sim.Trk #"<<iN<<" PDGid = "<<mcID<<endl;
      //   if(fabs(mcID)>1e4 || sumID>1e6) continue;
    
      //   TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(mcID);
      Double_t  fCharge = fParticle->Charge();
      TotCharge += fCharge;
    }
    // if(TotCharge!=0){
    //   cout<<"For Event #"<<j<<" tot. charge = "<<TotCharge<<endl;
    //   //  continue;
    // }
    if(verboseLevel==1){
      //  if(nGeaneTrks!=0 && sumID!=4424 && sumID>10){ 
	cout<<" "<<endl;
	cout<<"sumIDs="<<sumID<<endl;
	cout<<" ************************************** "<<endl;
	// }
    }
    ///-----------------------------------------------------------------------------------------
  
    //    if(sumID>0){//TEST
    for (Int_t iN=0; iN<nParticles; iN++){
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(iN);
	Int_t mcID = mctrk->GetPdgCode();
	if(fabs(mcID)>1e5) continue;
	TVector3 StartMC = mctrk->GetStartVertex();
	TVector3 MomMC = mctrk->GetMomentum();
	int nMChit = mctrk->GetNPoints(kLUMI);
	nmcall->Fill(mcID,sumID,nParticles,iN,StartMC.X(),StartMC.Y(),StartMC.Z(),
		     MomMC.X(),MomMC.Y(), MomMC.Z(),
		     MomMC.Mag(), MomMC.Theta(), MomMC.Phi(),nRecHits,TotCharge);
	}
    
    
    if(nGeaneTrks==0) continue;
    if(verboseLevel>2)  cout<<"Number of sim.particles: "<<nParticles<<" number of rec.trks: "<<numTrk<<" number of geane trks: "<<nGeaneTrks<<endl;
    /// Read INFO from reconstructed tracks ----------------------------------------------------
    if(verboseLevel>5) cout<<"== Read INFO from reconstructed tracks =="<<endl;
    glnrectrks = nGeaneTrks;
    glnrechits = rechit_array->GetEntriesFast();
    glSumID = sumID;
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
	Double_t errX = fRes->GetDX();
	Double_t errY = fRes->GetDY();
	Double_t errZ = fRes->GetDZ();

	Double_t thetaRec = TMath::Pi()/2. - lyambda;
	Double_t err_lyambda = fRes->GetDLambda();

	Double_t phiRec = fRes->GetPhi();
	Double_t err_phi = fRes->GetDPhi();

	TVector3 MomRec = fRes->GetMomentum();
	Double_t errMomRec = fRes->GetDQp();
	Double_t errPx = fRes->GetDPx();
	Double_t errPy = fRes->GetDPy();
	Double_t errPz = fRes->GetDPz();

	PndLinTrack *trk_lin = (PndLinTrack*)rec_trk->At(iN);
	Int_t candID = trk_lin->GetTCandID();
	glChi2 = trk_lin->GetChiSquare();
	PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);
	const int Ntrkcandhits= trkcand->GetNHits();
	if(verboseLevel>2)  cout<<"Trk #"<<iN<<" has "<<Ntrkcandhits<<" hits"<<endl;
	//	cout<<"trk #"<<iN<<": ";

	// //strip
	// for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){
	//   PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	//   Int_t hitID = candhit.GetHitId();
	//   PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	  
	//   ///Top cluster
	//   PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	//   PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	//   if (astripdigi->GetIndex(0) == -1) continue; // sort out noise
	//   PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	//   int MCidTOP = MCPoint->GetTrackID();
	  
	//   ///Bottom cluster
	//   Int_t  botIndex = myHit->GetBotIndex();
	//   PndSdsClusterStrip* myClusterBot =  (PndSdsClusterStrip*)(fStripClusterArray->At(botIndex));
	//   PndSdsDigiStrip* astripdigiBot = (PndSdsDigiStrip*)(fStripDigiArray->At(myClusterBot->GetDigiIndex(0)));
	//   if (astripdigiBot->GetIndex(0) == -1) continue; // sort out noise
	//   PndSdsMCPoint* MCPointBot = (PndSdsMCPoint*)(true_points->At(astripdigiBot->GetIndex(0)));
	//   int MCidBOT = MCPointBot->GetTrackID();
	//   //	if(MCidTOP!=MCidBOT) continue;
	//   MCtrkID.push_back(MCidTOP);
	//   MCtrkID.push_back(MCidBOT);
	//   MCtrkIDcount.push_back(1);
	//   MCtrkIDcount.push_back(1);
	//   //	  cout<<" "<<MCidTOP<<" "<<MCidBOT;
	// }

	//pixel
	  if(verboseLevel>5) 
	    cout<<"real hits size: "<<fStripClusterArray->GetEntriesFast()<<endl;
	for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){
	  //  if(verboseLevel>5) cout<<"Now working with hits!"<<endl;
	  PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	  //  if(verboseLevel>5) cout<<"Now working with hits! #2"<<endl;
	  Int_t hitID = candhit.GetHitId();
	  PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	  //	  if(verboseLevel>5) cout<<"Now working with hits! #3"<<endl;
	  PndSdsClusterPixel* myCluster = (PndSdsClusterPixel*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	  //  if(verboseLevel>5) cout<<"Now working with hits! #4"<<endl;
	  PndSdsDigiPixel* astripdigi = (PndSdsDigiPixel*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	  //  if(verboseLevel>5) cout<<"Now working with hits! #5"<<endl;
	  PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	  //  if(verboseLevel>5) cout<<"Now working with hits! #6"<<endl;
	  int MCidTOP = MCPoint->GetTrackID();
	  MCtrkID.push_back(MCidTOP);
	  MCtrkIDcount.push_back(1);
	  if(verboseLevel>1) cout<<MCidTOP<<" ";
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
	  //	  cout<<"======================"<<endl;
	  //	  cout<<"Number of sim.particles: "<<nParticles<<" number of rec.trks: "<<numTrk<<" number of geane trks: "<<nGeaneTrks<<endl;
	  //	  for(Int_t n=0; n<MCtrkID.size(); n++) {
	  //	    cout<<MCtrkID[n]<<" ";
	  //	  }
	  //	  cout<<""<<endl;
	  Int_t prevID = MCtrkID[0];
	  diffIDs = 1;
	  bool fillRecTrk=true;
	
	  for(Int_t n=1; n<MCtrkID.size(); n++){ 
	    if(verboseLevel>2)   cout<<MCtrkID[n]<<" =MCtrkID["<<n<<"]"<<endl;
	    if(prevID<MCtrkID[n]){
	      ///Fill MC hists --------------------------------------------------------
	      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(MCtrkID[n]);
	      Int_t mcID = mctrk->GetPdgCode();
	      if(fabs(mcID)>1e5) continue;
	      Int_t mcIDmother = mctrk->GetMotherID();
	      TVector3 MomMC = mctrk->GetMomentum();
	      TVector3 StartMC = mctrk->GetStartVertex();
	   
	      ///Fill histgs for antiproton------------------------
	      if(fillRecTrk){
		if(mcID==-2212 && sumID==4424) {
      
		  glThetaSig = MomRec.Theta();
		  glPhiSig = MomRec.Phi();
		  glXpcaSig = posRec.X();
		  glYpcaSig = posRec.Y();
		  glZpcaSig = posRec.Z();
		  glPxSig= MomRec.X();
		  glPySig = MomRec.Y();
		  glPzSig=MomRec.Z();
		  // glerrThetaSig = err_lyambda;
		  // glerrPhiSig = err_phi;
		  // glerrXpcaSig = errX;
		  // glerrYpcaSig = errY;
		  // glerrZpcaSig = errZ;
		  // glerrPxSig = MomRec.X();
		  // glerrPySig = MomRec.Y();
		  // glerrPzSig = MomRec.Z();
		  glPSig = MomRec.Mag();
		  //	glerrPSig = errMomRec;
		  tSig->Fill();
		  fillRecTrk=false;//put information from reconstructed track only once!
		}
		else{
		  glThetaBkg = MomRec.Theta();
		  glPhiBkg = MomRec.Phi();
		  glXpcaBkg = posRec.X();
		  glYpcaBkg = posRec.Y();
		  glZpcaBkg = posRec.Z();
		  glPxBkg= MomRec.X();
		  glPyBkg = MomRec.Y();
		  glPzBkg=MomRec.Z();
		  // glerrThetaBkg = err_lyambda;
		  // glerrPhiBkg = err_phi;
		  // glerrXpcaBkg = errX;
		  // glerrYpcaBkg = errY;
		  // glerrZpcaBkg = errZ;
		  // glerrPxBkg = MomRec.X();
		  // glerrPyBkg = MomRec.Y();
		  // glerrPzBkg = MomRec.Z();
		  glPBkg = MomRec.Mag();
		  //	  glerrPBkg = errMomRec;
		  glIDBkg = mcID;
		  glSumIDBkg = sumID;
		  glMotherIDBkg = mcIDmother;
		  tBkg->Fill();
		  fillRecTrk=false;//put information from reconstructed track only once!
		}
		glTheta = MomRec.Theta();
		glPhi = MomRec.Phi();
		glXpca = posRec.X();
		glYpca = posRec.Y();
		glZpca = posRec.Z();
		glPx= MomRec.X();
		glPy = MomRec.Y();
		glPz=MomRec.Z();
		glerrTheta = err_lyambda;
		glerrPhi = err_phi;
		glerrXpca = errX;
		glerrYpca = errY;
		glerrZpca = errZ;
		glerrPx = MomRec.X();
		glerrPy = MomRec.Y();
		glerrPz = MomRec.Z();
		glP = MomRec.Mag();
		glerrP = errMomRec;
		glXmc = StartMC.X();
		glYmc = StartMC.Y();
		glZmc = StartMC.Z();
		glPmc = MomMC.Mag();
		glThetamc = MomMC.Theta();
		glPhimc = MomMC.Phi();
		glID = mcID;
		glMotherID = mcIDmother;
		tAll->Fill();
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
	}
	  if(verboseLevel>2) {
	    cout<<"Trk #"<<iN<<" diffIDs = "<<diffIDs<<endl;
	    cout<<"MCtrk["<<iN<<"]="<<MCtrk[iN]<<endl;
	  }
	//	else{//reconstructed trk contains hits from one MC trks
	//reconstructed trk contains hits from one MC trks+ 1st Hit information for previous case
	    ///Fill MC histgs --------------------------------------------------------
	// if(verboseLevel>2) 
	//   cout<<"MCtrkID[0]="<<MCtrkID[0]<<endl;
	    PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(MCtrkID[0]);
	    Int_t mcID = mctrk->GetPdgCode();
	    if(fabs(mcID)>1e5) continue;

	    Int_t mcIDmother = mctrk->GetMotherID();
	    TVector3 MomMC = mctrk->GetMomentum();
       	    TVector3 StartMC = mctrk->GetStartVertex();
     
	    ///Fill histgs for antiproton from el.scattering ---
	    if(mcID==-2212 && sumID==4424) {
	   
	      glThetaSig = MomRec.Theta();
	      glPhiSig = MomRec.Phi();
	      glXpcaSig = posRec.X();
	      glYpcaSig = posRec.Y();
	      glZpcaSig = posRec.Z();
	      glPxSig= MomRec.X();
	      glPySig = MomRec.Y();
	      glPzSig=MomRec.Z();
	      // glerrThetaSig = err_lyambda;
	      // glerrPhiSig = err_phi;
	      // glerrXpcaSig = errX;
	      // glerrYpcaSig = errY;
	      // glerrZpcaSig = errZ;
		// glerrPxSig = MomRec.X();
		// glerrPySig = MomRec.Y();
		// glerrPzSig = MomRec.Z();
		glPSig = MomRec.Mag();
		//	glerrPSig = errMomRec;
		tSig->Fill();
	    }
	    else{
	      glThetaBkg = MomRec.Theta();
		glPhiBkg = MomRec.Phi();
		glXpcaBkg = posRec.X();
		glYpcaBkg = posRec.Y();
		glZpcaBkg = posRec.Z();
		glPxBkg= MomRec.X();
		glPyBkg = MomRec.Y();
		glPzBkg=MomRec.Z();
		// glerrThetaBkg = err_lyambda;
		// glerrPhiBkg = err_phi;
		// glerrXpcaBkg = errX;
		// glerrYpcaBkg = errY;
		// glerrZpcaBkg = errZ;
		// glerrPxBkg = MomRec.X();
		// glerrPyBkg = MomRec.Y();
		// glerrPzBkg = MomRec.Z();
		glPBkg = MomRec.Mag();
		//	glerrPBkg = errMomRec;
		glIDBkg = mcID;
		glSumIDBkg = sumID;
		glMotherIDBkg = mcIDmother;
		tBkg->Fill();
	    }
	    glTheta = MomRec.Theta();
	    glPhi = MomRec.Phi();
	    glXpca = posRec.X();
	    glYpca = posRec.Y();
	    glZpca = posRec.Z();
	    glPx= MomRec.X();
	    glPy = MomRec.Y();
	    glPz=MomRec.Z();
	    glerrTheta = err_lyambda;
	    glerrPhi = err_phi;
	    glerrXpca = errX;
	    glerrYpca = errY;
	    glerrZpca = errZ;
	    glerrPx = MomRec.X();
	    glerrPy = MomRec.Y();
	    glerrPz = MomRec.Z();
	    glP = MomRec.Mag();
	    glerrP = errMomRec;
	    glXmc = StartMC.X();
	    glYmc = StartMC.Y();
	    glZmc = StartMC.Z();
	    glPmc = MomMC.Mag();
	    glThetamc = MomMC.Theta();
	    glPhimc = MomMC.Phi();
	    glID = mcID;
	    glMotherID = mcIDmother;
	    tAll->Fill();
	  ///----------------------------------------------------------
	    //	}
    }
    ///-----------------------------------------------------------------------------------------
  }
  nmcall->Write();
  nsdshits->Write();
  tAll->Write();
  tSig->Write();
  tBkg->Write();
  f->Close();
  if(verboseLevel>0)  cout<<"Macro succsefully finished!"<<endl;
}
