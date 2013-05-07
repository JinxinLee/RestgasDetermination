//################################################################
//# Macro for study difference between MC and REC tracks (with simple case: 1 trk per event)
//# author: Anastasia Karavdina
//# date: 14/04/2013
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(simple_mc_rec_match simpleMCandRECmatch.C)
//# target_link_libraries(simple_mc_rec_match ${ROOT_LIBRARIES} Lmd GeoBase ParBase Geom PndData TrkBase  VMC EG GeomPainter generalTools FairTools  LmdReco LmdTrk Geane trackrep RecoHits genfitAdapters genfit SdsReco Sds Stt Fts Proof MathMore Minuit FairDB Base)
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./simple_mc_rec_match --help
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
  int dnu=0;//if>0 : use namespace for pixel
  std::string startStr="", momStr="", nStr="", pathStr="", verbStr="" , mcTrkStr="", useNewDStr="",usedMHStr="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    
    std::cout << "This is script for comparision reconstructed and simulated tracks with parameters\n"
	      <<"-s start event \n"
	      <<"-n Number of events \n"
	      <<"-mom Beam Momentum \n"
	      <<"-path path to the file(s) \n"
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
  nMCtracks=1;
  cout<<"====================================="<<endl;
  cout<<"some INFO about this macro params: "<<endl;
  cout<<"expected number of events: "<<nEvents<<endl;
  cout<<"Pbeam: "<<Plab<<endl;
  cout<<"Will be used Path: "<<storePath<<endl;
  cout<<"====================================="<<endl;
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
 
  TString geaneFile = storePath+"/Lumi_Geane_";
  geaneFile += startEvent;
  geaneFile += ".root";
  TChain tgeane("cbmsim");
  tgeane.Add(geaneFile);
  
  // ---------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/Lumi_compare_MC_and_REC_trks_";
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
  rechit_array = new TClonesArray("PndSdsMergedHit");
  tHitsMerged.SetBranchAddress("LMDHitsMerged",&rechit_array);  //Points for Tracks
  //----------------------------------------------------------------------------------
  
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  // TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  // tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  TClonesArray* rec_trk=new TClonesArray("PndTrack");
  tTrkRec.SetBranchAddress("LMDPndTrack",&rec_trk);  //Tracks
  //----------------------------------------------------------------------------------
  
  //--- Geane info ------------------------------------------------------------------
  TClonesArray* geaneArray =new TClonesArray("FairTrackParH");
  tgeane.SetBranchAddress("GeaneTrackFinal",&geaneArray);  //Tracks with helix parametrisation
  
  cout<<"And we'll make some hists"<<endl;
  //--- Output histogram -----------------------------------------------------
  double thetarange[2]={0.001,0.01};
  double thetam =  thetarange[0];
  if(Plab<5) thetam =  thetarange[1];

  //Near IP
  TH1 *hResMom = new TH1F("hResMom","P_{MC}-P_{REC};#deltaP,GeV/c",1e3,-1e-4,1e-4);
  TH1 *hErrMom = new TH1F("hErrMom","#sigma_{P};#sigmaP,GeV/c",1e3,0,1e-3);
  TH1 *hPullMom = new TH1F("hPullMom","(P_{MC}-P_{REC})/#sigma_{P};",1e3,-1e1,1e1);
  //  TH1 *hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{REC};#delta#theta,rad",1e3,-1e-2,1e-2);
  TH1 *hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{REC};#delta#theta,rad",1e2,-thetam,thetam);//TEST
  TH2 *hThetaResTheta = new TH2F("hThetaResTheta","#theta_{MC}-#theta_{REC};#theta,rad;#delta#theta,rad",2e1,0,1e-2,1e2,-thetam,thetam);//TEST
  TH1 *hErrTheta = new TH1F("hErrTheta","#sigma(#theta_{REC});#sigma,rad",1e3,0,10*thetam);
  TH1 *hPullTheta = new TH1F("hPullTheta","(#theta_{MC}-#theta_{REC})/#sigma_{#theta};",1e2,-10,10);
  TH1 *hResPhi = new TH1F("hResPhi","#phi_{MC}-#phi_{REC};#delta#phi,rad",2e3,-1.,1.);
  TH1 *hErrPhi = new TH1F("hErrPhi","#sigma(#phi_{REC});#sigma,rad",1e3,0,0.1);
  TH1 *hPullPhi = new TH1F("hPullPhi","(#phi_{MC}-#phi_{REC})/#sigma_{#phi};",1e2,-10,10);

  TH1 *hResPointPx = new TH1F("hResPointPx","Px_{MC}-Px_{REC};#deltaPx, GeV/c",1e2,-0.01,0.01);
  TH1 *hErrPointPx = new TH1F("hErrPointPx","#sigma_{Px};#sigmaPx, GeV/c",1e3,0,0.01);
  TH1 *hPullPointPx = new TH1F("hPullPointPx","(Px_{MC}-Px_{REC})/#sigma_{Px};(Px_{MC}-Px_{REC})/#sigma_{Px}",1e2,-10,10);
  
  TH1 *hResPointPy = new TH1F("hResPointPy","Py_{MC}-Py_{REC};#deltaPy, GeV/c",1e2,-0.01,0.01);
  TH1 *hErrPointPy = new TH1F("hErrPointPy","#sigma_{Py};#sigmaPy, GeV/c",1e3,0,0.01);
  TH1 *hPullPointPy = new TH1F("hPullPointPy","(Py_{MC}-Py_{REC})/#sigma_{Py};(Py_{MC}-Py_{REC})/#sigma_{Py}",1e2,-10,10);
  
  TH1 *hResPointPz = new TH1F("hResPointPz","Pz_{MC}-Pz_{REC};#deltaPz, GeV/c",1e2,-1e-3,1e-3);
  TH1 *hErrPointPz = new TH1F("hErrPointPz","#sigma_{Pz};#sigmaPz, GeV/c",1e3,0,1e-2);
  TH1 *hPullPointPz = new TH1F("hPullPointPz","(Pz_{MC}-Pz_{REC})/#sigma_{Pz};(Pz_{MC}-Pz_{REC})/#sigma_{Pz}",1e2,-10,10);

  TH1 *hResPointX = new TH1F("hResPointX","X_{MC}-X_{REC};#deltaX,cm",1e2,-2.,2.);
  TH1 *hPullPointX = new TH1F("hPullPointX","(X_{MC}-X_{REC})/#sigma_{X};(X_{MC}-X_{REC})/#sigma_{X}",1e2,-10,10);
  TH1 *hResPointY = new TH1F("hResPointY","Y_{MC}-Y_{REC};#deltaY,cm",1e2,-2.,2.);
  TH1 *hPullPointY = new TH1F("hPullPointY","(Y_{MC}-Y_{REC})/#sigma_{Y};(Y_{MC}-Y_{REC})/#sigma_{Y}",1e2,-10,10);
  TH1 *hResPointZ = new TH1F("hResPointZ","Z_{MC}-Z_{REC};#deltaZ,cm",1e3,-0.15,0.15);
  TH1 *hPullPointZ = new TH1F("hPullPointZ","(Z_{MC}-Z_{REC})/#sigma_{Z};(Z_{MC}-Z_{REC})/#sigma_{Z}",1e2,-10,10);

  //Near 1st LMD plane
  TH1 *hhits = new TH1I("hhits","number of hits in trk",7,0,7);
  TH1 *hchi2 = new TH1F("hchi2","#chi^2 for reconstructed tracks;#chi^2;",1.5e2,0,15.);
  TH1 *hResLumiTrkMom = new TH1F("hResLumiTrkMom","P_{MC}-P_{REC}(near Lumi);#deltaP,GeV/c",1e3,-6e-7,6e-7);
  TH1 *hResLumiTrkTheta = new TH1F("hResLumiTrkTheta","#theta_{MC}-#theta_{REC}(near Lumi);#delta#theta,rad",1e3,-6e-3,6e-3);
  TH1 *hResLumiTrkPhi = new TH1F("hResLumiTrkPhi","#phi_{MC}-#phi_{REC}(near Lumi);#delta#phi,rad",2e3,-1e-1,1e-1);
  TH1 *hResLumiTrkPointX = new TH1F("hResLumiTrkPointX","X_{MC}-X_{REC}(near Lumi);#deltaX,cm",1e2,-0.02,0.02);
  TH1 *hResLumiTrkPointY = new TH1F("hResLumiTrkPointY","Y_{MC}-Y_{REC}(near Lumi);#deltaY,cm",1e2,-0.02,0.02);
  TH1 *hResLumiTrkPointZ = new TH1F("hResLumiTrkPointZ","Z_{MC}-Z_{REC}(near Lumi);#deltaZ,cm",1e2,-0.02,0.02);

  TH1 *hResLumiTrkPointPx = new TH1F("hResLumiTrkPointPx","Px_{MC}-Px_{REC}(near Lumi);#deltaPx, GeV/c",1e2,-0.01,0.01);
  TH1 *hResLumiTrkPointPy = new TH1F("hResLumiTrkPointPy","Py_{MC}-Py_{REC}(near Lumi);#deltaPy, GeV/c",1e2,-0.01,0.01);
  TH1 *hResLumiTrkPointPz = new TH1F("hResLumiTrkPointPz","Pz_{MC}-Pz_{REC}(near Lumi);#deltaPz, GeV/c",1e2,-0.001,0.001);
  //  TH1 *hResLumiTrkPointP = new TH1F("hResLumiTrkPointP","P_{MC}-P_{REC}(near Lumi);#deltaP, GeV/c",1e2,-0.001,0.001);
  //  TH2 *hResLumiTrkPointPmcPrec = new TH2F("hResLumiTrkPointPmcPrec","P_{MC} vs. P_{REC}(near Lumi);P_{rec}, GeV/c;P_{mc}, GeV/c",1e2,Plab-0.1,Plab+0.1,1e2,Plab-0.1,Plab+0.1);

 TH1 *hResLumiTrkPointXErr = new TH1F("hResLumiTrkPointXErr","#sigma(X_{REC})(near Lumi);#sigma_{X},cm",1e2,0,0.02);
  TH1 *hResLumiTrkPointYErr = new TH1F("hResLumiTrkPointYErr","#sigma(Y_{REC})(near Lumi);#sigma_{Y},cm",1e2,0,0.02);
  TH1 *hResLumiTrkPointZErr = new TH1F("hResLumiTrkPointZErr","#sigma(Z_{REC})(near Lumi);#sigma_{Z},cm",1e2,0,0.02);
  TH1 *hResLumiTrkPointPxErr = new TH1F("hResLumiTrkPointPxErr","#sigma(Px_{REC})(near Lumi);#sigma_{Px}, GeV/c",1e2,0,0.01);
  TH1 *hResLumiTrkPointPyErr = new TH1F("hResLumiTrkPointPyErr","#sigma(Py_{REC})(near Lumi);#sigma_{Py}, GeV/c",1e2,0,0.01);
  TH1 *hResLumiTrkPointPzErr = new TH1F("hResLumiTrkPointPzErr","#sigma(Pz_{REC})(near Lumi);#sigma_{Pz}, GeV/c",1e2,0,0.001);

  TH1 *hResLumiTrkPointXPull = new TH1F("hResLumiTrkPointXPull","(X_{MC}-X_{REC})/#sigma (near Lumi) ;(X_{MC}-X_{REC})/#sigma",1e2,-10.,10.);
  TH1 *hResLumiTrkPointYPull = new TH1F("hResLumiTrkPointYPull","(Y_{MC}-Y_{REC})/#sigma (near Lumi);(Y_{MC}-Y_{REC})/#sigma",1e2,-10.,10.);
  TH1 *hResLumiTrkPointZPull = new TH1F("hResLumiTrkPointZPull","(Z_{MC}-Z_{REC})/#sigma (near Lumi);(Z_{MC}-Z_{REC})/#sigma",1e3,-100.,100.);

  TH1 *hResLumiTrkPointPxPull = new TH1F("hResLumiTrkPointPxPull","(Px_{MC}-Px_{REC})/#sigma (near Lumi);(Px_{MC}-Px_{REC})/#sigma",1e2,-10,10.);
  TH1 *hResLumiTrkPointPyPull = new TH1F("hResLumiTrkPointPyPull","(Py_{MC}-Py_{REC})/#sigma (near Lumi);(Py_{MC}-Py_{REC})/#sigma",1e2,-10,10);
  TH1 *hResLumiTrkPointPzPull = new TH1F("hResLumiTrkPointPzPull","(Pz_{MC}-Pz_{REC})/#sigma (near Lumi);(Pz_{MC}-Pz_{REC})/#sigma",1e2,-10,10);
  TH1 *hResLumiTrkThetaPull = new TH1F("hResLumiTrkThetaPull","(#theta_{MC}-#theta_{REC})/#sigma (near Lumi);#delta#theta, rad",1e2,-10,10);
  TH1 *hResLumiTrkPhiPull = new TH1F("hResLumiTrkPhiPull","(#phi_{MC}-#phi_{REC})/#sigma (near Lumi);#delta#phi, rad",1e2,-10,10);

  //  TH2 *hMCidRefID = new TH2I("hMCidRefID","; MCid; RefID",100,0,100,100,0,100);

  TNtuple *nBadTrks = new TNtuple("nBadTrks","Info about _bad_ rec.tracks ","xrec:yrec:zrec:pxrec:pyrec:pzrec:nrechits:xmc:ymc:zmc:pxmc:pymc:pzmc:mvID");

  int glBADGEANE=0;
  //  int glBadEv = 0;
  //  int glNoisehit = 0;// total number of noise hits
  for (Int_t j=0; j<nEvents; j++){
    // Read GEANE & MC info -----------------------------------------------------------------
    //    if(kf<1) 
    tTrkRec.GetEntry(j);
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
    tTrkCand.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);
    tHitsMerged.GetEntry(j);
    const int nGeaneTrks = geaneArray->GetEntriesFast();
    const int nParticles = true_tracks->GetEntriesFast();
    const int numTrk = nGeaneTrks;
    const int nRecHits = rechit_array->GetEntriesFast();
    const int nMCHits = true_points->GetEntriesFast();
    
    const int nTrkCandidates = trkcand_array->GetEntriesFast();
    const int nRecTrks = rec_trk->GetEntriesFast();
    if(verboseLevel>0)  
      cout<<"Event #"<<j<<" has "<<nParticles<<" true particles, "<<" out of it "<<nRecHits<<" hits, "<<nTrkCandidates
	  <<" trk-cands, "<<numTrk<<" tracks and "<<nGeaneTrks<<" geane Trks!"<<endl;
    //    if(nParticles!=nMCtracks) continue;
    for (Int_t iN=0; iN<nGeaneTrks; iN++){// loop over all reconstructed trks
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(iN);
      Double_t lyambda = fRes->GetLambda();
      if(lyambda==0){
	cout<<"GEANE didn't propagate this trk!"<<endl;
	glBADGEANE++;
	nBadTrks->Fill(1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6);
      }
      if(lyambda==0) continue;

      /// Read REC track parameters near IP -----------------------------------
      TVector3 MomRecPCA = fRes->GetMomentum();
      TVector3 MomRecPCAnotnorm = MomRecPCA;
      MomRecPCA *= Plab/MomRecPCA.Mag();
      TVector3 PosRecPCA = fRes->GetPosition();
      Double_t errPx = fRes->GetDPx();
      Double_t errPy = fRes->GetDPy();
      Double_t errPz = fRes->GetDPz();
      TVector3 errMomRecPCA(errPx,errPy,errPz);
      Double_t errX = fRes->GetDX();
      Double_t errY = fRes->GetDY();
      Double_t errZ = fRes->GetDZ();
      TVector3 errPosRecPCA(errX,errY,errZ);

      Double_t thetaBP = TMath::Pi()/2. - lyambda;
      //   Double_t err_lyambda = fRes->GetDLambda();
      Double_t phiBP = fRes->GetPhi();
      //  Double_t err_phi = fRes->GetDPhi();

      //calculate theta & phi errors
      double fLmPCA = TMath::ASin(MomRecPCA.Z()/MomRecPCA.Mag());
      double cLmPCA= TMath::Cos(fLmPCA);
      double  sLmPCA= TMath::Sin(fLmPCA);
      Double_t fPPCA =sqrt(MomRecPCA.X()*MomRecPCA.X()+MomRecPCA.Y()*MomRecPCA.Y()+MomRecPCA.Z()*MomRecPCA.Z());
      Double_t fDPPCA= (2*MomRecPCA.X()*errMomRecPCA.X()+2*MomRecPCA.Y()*errMomRecPCA.Y()+2*MomRecPCA.Z()*errMomRecPCA.Z())/(2*fPPCA); //dp
      Double_t err_lyambda = (-((MomRecPCA.Z()*fDPPCA)/pow(fPPCA,2)) + errMomRecPCA.Z()/fPPCA)/ TMath::Sqrt(1 - pow(MomRecPCA.Z(),2)/pow(fPPCA,2)); 
      Double_t err_phi = (-((MomRecPCA.Y()*fDPPCA/cLmPCA)/pow(fPPCA,2)) + (errMomRecPCA.Y()/cLmPCA)/fPPCA +(MomRecPCA.Y()*err_lyambda*TMath::Tan(fLmPCA)/cLmPCA)/fPPCA) /TMath::Sqrt(1 - (pow(MomRecPCA.Y(),2)*pow(1/cLmPCA,2))/pow(fPPCA,2)); 


	// Double_t CovGEANELAB[6][6];
	// fRes->GetMARSCov(CovGEANELAB);
	//	Double_t errMomRecBP = fRes->GetDQp();
      // ///get rid from most probably ghost track ----------
      // //TODO: find reason for such trks in Kalman
      // double pca_lim = 1.;//=10*sigma_Xpca~10*{0.093,0.11,0.12,0.22,0.55};
      // if(Plab<5) pca_lim = 2.;
      // if(Plab<2) pca_lim = 5.;
      // if(fabs(PosRecPCA.X())>pca_lim && fabs(PosRecPCA.Y())>pca_lim) continue; // PCA_x and PCA_y should be < 10sigmaX
      // ///get rid from most probably ghost track (END) ---     
      // ///------------------------------------------------------------------------------------

      /// Read REC track parameters near LMD -----------------------------------
      PndTrack *trkpnd = (PndTrack*)rec_trk->At(iN);
      double chi2 = trkpnd->GetChi2();
      hchi2->Fill(chi2);
      FairTrackParP fFittedTrkP = trkpnd->GetParamFirst();
      TVector3 PosRecLMD(fFittedTrkP.GetX(),fFittedTrkP.GetY(),fFittedTrkP.GetZ());
      TVector3 MomRecLMD(fFittedTrkP.GetPx(),fFittedTrkP.GetPy(),fFittedTrkP.GetPz());
      MomRecLMD *=Plab/MomRecLMD.Mag();
      double covMARS[6][6];
      fFittedTrkP.GetMARSCov(covMARS);
      TVector3 errMomRecLMD(sqrt(covMARS[0][0]),sqrt(covMARS[1][1]),sqrt(covMARS[2][2]));
      TVector3 errPosRecLMD(sqrt(covMARS[3][3]),sqrt(covMARS[4][4]),sqrt(covMARS[5][5]));

      //calculate theta & phi errors
      double fLm = TMath::ASin(MomRecLMD.Z()/MomRecLMD.Mag());
      double cLm= TMath::Cos(fLm);
      double  sLm= TMath::Sin(fLm);
      Double_t fP =sqrt(MomRecLMD.X()*MomRecLMD.X()+MomRecLMD.Y()*MomRecLMD.Y()+MomRecLMD.Z()*MomRecLMD.Z());
      Double_t fDP= (2*MomRecLMD.X()*errMomRecLMD.X()+2*MomRecLMD.Y()*errMomRecLMD.Y()+2*MomRecLMD.Z()*errMomRecLMD.Z())/(2*fP); //dp
      Double_t err_lyambdaLMD = (-((MomRecLMD.Z()*fDP)/pow(fP,2)) + errMomRecLMD.Z()/fP)/ TMath::Sqrt(1 - pow(MomRecLMD.Z(),2)/pow(fP,2)); 
      Double_t err_phiLMD = (-((MomRecLMD.Y()*fDP/cLm)/pow(fP,2)) + (errMomRecLMD.Y()/cLm)/fP +(MomRecLMD.Y()*err_lyambdaLMD*TMath::Tan(fLm)/cLm)/fP) /TMath::Sqrt(1 - (pow(MomRecLMD.Y(),2)*pow(1/cLm,2))/pow(fP,2)); 
      ///---------------------------------------------------------------------------------------

  
  	
	//Matching between MC & Rec on 1st hit level-----------------------------------
      int candID = trkpnd->GetRefIndex();
      PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);    
      const int Ntrkcandhits= trkcand->GetNHits();
      PndSdsMCPoint* MCPointHit;
      int MCid;
      bool hitmix = false;
      if(Ntrkcandhits<4) continue; //require trks with hits on all planes
      hhits->Fill(Ntrkcandhits);
      for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){ // loop over rec.hits
	PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	Int_t hitID = candhit.GetHitId();
	PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	
	//for pixel design
	PndSdsClusterPixel* myCluster = (PndSdsClusterPixel*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	PndSdsDigiPixel* astripdigi = (PndSdsDigiPixel*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	if (astripdigi->GetIndex(0) == -1)
	  continue;
	PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	int MCidTOP = MCPoint->GetTrackID();
	//	hMCidRefID->Fill(MCidTOP,myHit->GetRefIndex());
	if(iHit<1){
	  MCPointHit = MCPoint;
	  MCid = MCidTOP;
	}
	else
	  if(MCid!=MCidTOP){
	    cout<<"REC trk contains hits from different MC trks! Skip this event."<<endl;
	    hitmix = true;
	  }
      }
      if(hitmix) continue;
      ///--------------------------------------------------------------------------

      
      /// Comporision between MC tracks, reconstructed tracks near LMD  and back propagated tracks -------------

	/// Read MC track parameters near IP ------------------------------------
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(MCid);
	Int_t mcID = mctrk->GetPdgCode();
	int mvID = mctrk->GetMotherID();
	TVector3 MomMCpca = mctrk->GetMomentum();
	TVector3 PosMCpca = mctrk->GetStartVertex();
	Double_t thetaMC = MomMCpca.Theta();
	Double_t phiMC = MomMCpca.Phi();
	///------------------------------------------------------------------------------------
	hResPointPx->Fill(MomMCpca.X()-MomRecPCA.X());
	hResPointPy->Fill(MomMCpca.Y()-MomRecPCA.Y());
	hResPointPz->Fill(MomMCpca.Z()-MomRecPCA.Z());
	hErrPointPx->Fill(errMomRecPCA.X());
	hErrPointPy->Fill(errMomRecPCA.Y());
	hErrPointPz->Fill(errMomRecPCA.Z());
	hPullPointPx->Fill((MomMCpca.X()-MomRecPCA.X())/errMomRecPCA.X());
	hPullPointPy->Fill((MomMCpca.Y()-MomRecPCA.Y())/errMomRecPCA.Y());
	hPullPointPz->Fill((MomMCpca.Z()-MomRecPCA.Z())/errMomRecPCA.Z());
	hResPointX->Fill(PosMCpca.X()-PosRecPCA.X());
	hResPointY->Fill(PosMCpca.Y()-PosRecPCA.Y());
	hResPointZ->Fill(PosMCpca.Z()-PosRecPCA.Z());
	hPullPointX->Fill((PosMCpca.X()-PosRecPCA.X())/errPosRecPCA.X());
	hPullPointY->Fill((PosMCpca.Y()-PosRecPCA.Y())/errPosRecPCA.Y());
	hPullPointZ->Fill((PosMCpca.Z()-PosRecPCA.Z())/errPosRecPCA.Z());

	hResTheta->Fill(MomMCpca.Theta()-MomRecPCA.Theta());
	hThetaResTheta->Fill(MomMCpca.Theta(),(MomMCpca.Theta()-MomRecPCA.Theta()));
	hResPhi->Fill(MomMCpca.Phi()-MomRecPCA.Phi());
	hPullTheta->Fill((MomMCpca.Theta()-MomRecPCA.Theta())/err_lyambda);
	hPullPhi->Fill((MomMCpca.Phi()-MomRecPCA.Phi())/err_phi);
	//TNtuple *nBadTrks = new TNtuple("nBadTrks","Info about _bad_ rec.tracks ","xrec:yrec:zrec:pxrec:pyrec:pzrec:nrechits:xmc:ymc:zmc:pxmc:pymc:pzmc");
	if(fabs(MomRecPCAnotnorm.Mag()-Plab)>0.1*Plab){
	  nBadTrks->Fill(PosRecPCA.X(),PosRecPCA.Y(),PosRecPCA.Z(),MomRecPCAnotnorm.X(),MomRecPCAnotnorm.Y(),MomRecPCAnotnorm.Z(),Ntrkcandhits,PosMCpca.X(),PosMCpca.Y(),PosMCpca.Z(),MomMCpca.X(),MomMCpca.Y(),MomMCpca.Z(),mvID);
	  cout<<"Event #"<<j<<" contains BAD trk"<<endl;
	}

	//Near 1st LMD plane
	/// Read MC track parameters near LMD ------------------------------------
	TVector3 PosMClmd =  MCPointHit->GetPosition();
	double pxTrue =  MCPointHit->GetPx();
	double pyTrue =  MCPointHit->GetPy();
	double pzTrue =  MCPointHit->GetPz();
	TVector3 MomMClmd(pxTrue,pyTrue,pzTrue);
	TVector3 dirMClmd = MomMClmd;
	dirMClmd *=1./MomMClmd.Mag();
	double deltaZ = -PosMClmd.Z()+PosRecLMD.Z();
	//	double deltaZ = 0;
	double xneu=PosMClmd.X()+dirMClmd.X()*deltaZ;
	double yneu=PosMClmd.Y()+dirMClmd.Y()*deltaZ;
	double zneu = PosMClmd.Z()+deltaZ;
	PosMClmd.SetXYZ(xneu,yneu,zneu);
	MomMClmd = dirMClmd*Plab;
	//	hResLumiTrkPointP->Fill((MomMClmd.Mag()-MomRecLMD.Mag()));
	//	hResLumiTrkPointPmcPrec->Fill(MomRecLMD.Mag(),MomMClmd.Mag());


	// MomMClmd *=1./MomMClmd.Mag();//TEST
	// MomRecLMD *=1./MomRecLMD.Mag();//TEST
	// errMomRecLMD *=1./MomRecLMD.Mag();//TEST

	// cout<<"MomMClmd.Mag() = "<<MomMClmd.Mag()<<" MomRecLMD.Mag() = "<<MomRecLMD.Mag()<<endl;
	// cout<<" MC - REC = "<<1e3*(MomMClmd.Mag()-MomRecLMD.Mag())<<" MeV"<<endl;
	///------------------------------------------------------------------------------------
	hResLumiTrkPointX->Fill(PosMClmd.X()-PosRecLMD.X());
	hResLumiTrkPointY->Fill(PosMClmd.Y()-PosRecLMD.Y());
	hResLumiTrkPointZ->Fill(PosMClmd.Z()-PosRecLMD.Z());
	hResLumiTrkPointXPull->Fill((PosMClmd.X()-PosRecLMD.X())/errPosRecLMD.X());
	hResLumiTrkPointYPull->Fill((PosMClmd.Y()-PosRecLMD.Y())/errPosRecLMD.Y());
	hResLumiTrkPointZPull->Fill((PosMClmd.Z()-PosRecLMD.Z())/errPosRecLMD.Z());

	hResLumiTrkPointPx->Fill(MomMClmd.X()-MomRecLMD.X());
	hResLumiTrkPointPy->Fill(MomMClmd.Y()-MomRecLMD.Y());
	hResLumiTrkPointPz->Fill(MomMClmd.Z()-MomRecLMD.Z());
	hResLumiTrkPointPxPull->Fill((MomMClmd.X()-MomRecLMD.X())/errMomRecLMD.X());
	hResLumiTrkPointPyPull->Fill((MomMClmd.Y()-MomRecLMD.Y())/errMomRecLMD.Y());
	hResLumiTrkPointPzPull->Fill((MomMClmd.Z()-MomRecLMD.Z())/errMomRecLMD.Z());
	hResLumiTrkTheta->Fill(MomMClmd.Theta()-MomRecLMD.Theta());
	hResLumiTrkPhi->Fill(MomMClmd.Phi()-MomRecLMD.Phi());
	hResLumiTrkThetaPull->Fill((MomMClmd.Theta()-MomRecLMD.Theta())/err_lyambdaLMD);
	hResLumiTrkPhiPull->Fill((MomMClmd.Phi()-MomRecLMD.Phi())/err_phiLMD);
	hResLumiTrkPointXErr->Fill(errPosRecLMD.X());
	hResLumiTrkPointYErr->Fill(errPosRecLMD.Y());
	hResLumiTrkPointZErr->Fill(errPosRecLMD.Z());
	hResLumiTrkPointPxErr->Fill(errMomRecLMD.X());
	hResLumiTrkPointPyErr->Fill(errMomRecLMD.Y());
	hResLumiTrkPointPzErr->Fill(errMomRecLMD.Z());

      /// Comporision between MC tracks, reconstructed tracks near LMD  and back propagated tracks (END) -----
    }
  }

  //Draw results on few canvases

  TCanvas *c1 = new TCanvas("pulls_before_bp");
  c1->Divide(3,2);
  c1->cd(1);
  hResLumiTrkPointXPull->Draw();
  c1->cd(2);
  hResLumiTrkPointYPull->Draw();
  c1->cd(3);
  hResLumiTrkPointZPull->Draw();
  c1->cd(4);
  hResLumiTrkPointPxPull->Draw();
  c1->cd(5);
  hResLumiTrkPointPyPull->Draw();
  c1->cd(6);
  hResLumiTrkPointPzPull->Draw();
  c1->Write();
  c1->Close();

  TCanvas *c2 = new TCanvas("pulls_after_bp");
  c2->Divide(3,2);
  c2->cd(1);
  hPullPointX->Draw();
  c2->cd(2);
  hPullPointY->Draw();
  c2->cd(3);
  hPullPointZ->Draw();
  c2->cd(4);
  hPullPointPx->Draw();
  c2->cd(5);
  hPullPointPy->Draw();
  c2->cd(6);
  hPullPointPz->Draw();
  c2->Write();
  c2->Close();

  hchi2->Write();
  hResPointPx->Write();
  hResPointPy->Write();
  hResPointPz->Write();
  hErrPointPx->Write();
  hErrPointPy->Write();
  hErrPointPz->Write();
  hPullPointPx->Write();
  hPullPointPy->Write();
  hPullPointPz->Write();
  hResPointX->Write();
  hResPointY->Write();
  hResPointZ->Write();
  hPullPointX->Write();
  hPullPointY->Write();
  hPullPointZ->Write();
  hResLumiTrkPointX->Write();
  hResLumiTrkPointY->Write();
  hResLumiTrkPointZ->Write();
  hResLumiTrkPointXPull->Write();
  hResLumiTrkPointYPull->Write();
  hResLumiTrkPointZPull->Write();
  hResLumiTrkPointPx->Write();
  hResLumiTrkPointPy->Write();
  hResLumiTrkPointPz->Write();
  hResLumiTrkPointPxPull->Write();
  hResLumiTrkPointPyPull->Write();
  hResLumiTrkPointPzPull->Write();
  hResTheta->Write();
  hResPhi->Write();
  hPullTheta->Write();
  hPullPhi->Write();
  hResLumiTrkTheta->Write();
  hResLumiTrkPhi->Write();
  hResLumiTrkThetaPull->Write();
  hResLumiTrkPhiPull->Write();
  hhits->Write();
  hResLumiTrkPointXErr->Write();
  hResLumiTrkPointYErr->Write();
  hResLumiTrkPointZErr->Write();
  hResLumiTrkPointPxErr->Write();
  hResLumiTrkPointPyErr->Write();
  hResLumiTrkPointPzErr->Write();
  //  hResLumiTrkPointP->Write();
  //  hResLumiTrkPointPmcPrec->Write();
  //  hMCidRefID->Write();
  hThetaResTheta->Write();
  nBadTrks->Write();
  f->Close();
 cout<<"Number of trks where GEANE failed: "<<glBADGEANE<<endl;
}
