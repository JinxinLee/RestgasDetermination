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
// void combitransFromLumiFrame(TVector3& hitPos, int senType=0){
//   Double_t  kTransZ, kRotUmZ, kRot, kTransX;
//   if(senType>0){ //Pixel sensors
//     //do the transformation from lab frame to LUMI frame (with z-axis perp. to lumi planes)
//     double end_seg_upstream = 360.1; // where bending starts with
//     double r_bend = 5750.; // the bending radius
//     double phi_bend = 40.068e-3; // and the angle of the circle path
//     // const Double_t kRot = phi_bend/3.141*180.;//=2.295727;//2.326; //(deg) //Rotate to dipol
//     kRot = phi_bend;//here we need abgle in rad!
//     // the point where both tangents of the straight beam pipe tubes meet is
//     kRotUmZ = end_seg_upstream + tan(phi_bend/2.)*r_bend;//476.03; //(cm) //z-point to rotate
//     kTransZ = 1130.; //(cm) //move at z-position
//     kTransX = (kTransZ - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend); // 25 (cm) //move at x-position
//   }
//   else{
//     /// THIS WORKS ONLY FOR DESIGN WITH STRIP SENSORS!
//     //do the transformation from lab frame to LUMI frame (with z-axis perp. to lumi planes)
//     kTransZ = 1100.; //(cm) //move at z-position
//     kRotUmZ = 476.03; //(cm) //z-point to rotate
//     kTransX = 25; //(cm) //move at x-position
//     kRot =  0.040596358401388; // 2.326 degree  = 4.05963584013881024e-02 rad
//   }
//   TVector3 LumiTrans(0,0,kTransZ-kRotUmZ);
//   hitPos +=LumiTrans;
//   hitPos.RotateY(kRot);
//   LumiTrans = TVector3(0,0,kRotUmZ);
//   hitPos +=LumiTrans;
// }
// TMatrixD rotateFromLumiFrame(TMatrixD& hitCov, int senType=0){
//   Double_t  sintheta, costheta;
//   if(senType>0){ //Pixel sensors
//     double phi_bend = 40.068e-3; // and the angle of the circle path
//     sintheta = TMath::Sin(phi_bend);
//     costheta = TMath::Cos(phi_bend);
//   }
//   else{
//     /// THIS WORKS ONLY FOR DESIGN WITH STRIP SENSORS!
//     Double_t theta=-2.326;
//     Double_t degrad = TMath::Pi()/180.;
//     sintheta = TMath::Sin(degrad*theta);
//     costheta = TMath::Cos(degrad*theta);
//   }
//   TMatrixD rot(3,3);// Rotation around Y axis
//   rot[0][0]= costheta;
//   rot[0][1]= 0;
//   rot[0][2]= sintheta;
//   rot[1][0]= 0;
//   rot[1][1]= 1;
//   rot[1][2]= 0;
//   rot[2][0]= -sintheta;
//   rot[2][1]= 0;
//   rot[2][2]= costheta;
//   TMatrixD result = rot;
//   result.T();
//   result*=hitCov;
//   hitCov = result;
//   result*=rot;
//   return result;
// }


// void rotateFromLumiFrame(TVector3& hitPos, bool err, int senType=0){
//   TMatrixD hitMtx(3,3);
//   if(err){
//     hitMtx[0][0] = hitPos[0]*hitPos[0];
//     hitMtx[1][1] = hitPos[1]*hitPos[1];
//     hitMtx[2][2] = hitPos[2]*hitPos[2];
//     TMatrixD res = rotateFromLumiFrame(hitMtx,senType);
//     hitPos = TVector3(sqrt(res(0,0)),sqrt(res(1,1)),sqrt(res(2,2)));
//   }
//   else{
//     hitMtx[0][0] = hitPos[0];
//     hitMtx[1][0] = hitPos[1];
//     hitMtx[2][0] = hitPos[2];
//     TMatrixD res = rotateFromLumiFrame(hitMtx,senType);
//     hitPos = TVector3(hitMtx(0,0),hitMtx(1,0),hitMtx(2,0));
//   }
// }

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
	      <<"-t Number of trks per event \n"
	      <<"-mom Beam Momentum \n"
	      <<"-path path to the file(s) \n"
	      <<"-v verbose Level (if>0, print out some information) \n"
	      <<"-npx if>0: use namespace for pixel \n"
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
    if (sw=="-npx"){
      optind++;
      useNewDStr = __argv[optind];
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

  std::stringstream startSStr(startStr), momSStr(momStr), nSStr(nStr), pathSStr(pathStr), verbSStr(verbStr), mcTrkSStr(mcTrkStr),useNewDSStr(useNewDStr), usedMHSStr(usedMHStr);

  startSStr >> startEvent;
  momSStr >> Plab;
  nSStr >> nEvents;
  pathSStr >> storePath;
  verbSStr >> verboseLevel;
  mcTrkSStr >> nMCtracks;
  useNewDSStr >> dnu;
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
  if(dnu>0){
    fStripClusterArray = new TClonesArray("PndSdsClusterPixel");
    tHits.SetBranchAddress("LMDPixelClusterCand",&fStripClusterArray);
  }
  else{
    fStripClusterArray = new TClonesArray("PndSdsClusterStrip");
    tHits.SetBranchAddress("LMDStripClusterCand",&fStripClusterArray); 
  }

  
  TClonesArray* fStripDigiArray;
  if(dnu>0){
    fStripDigiArray = new TClonesArray("PndSdsDigiPixel");
    tdigiHits.SetBranchAddress("LMDPixelDigis",&fStripDigiArray);
  }
  else{
    fStripDigiArray = new TClonesArray("PndSdsDigiStrip");
    tdigiHits.SetBranchAddress("LMDStripDigis",&fStripDigiArray); 
  }
  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array;
  if(dnu>0){
    if(mh>0){  
      rechit_array = new TClonesArray("PndSdsMergedHit");
      tHitsMerged.SetBranchAddress("LMDHitsMerged",&rechit_array);  //Points for Tracks
    }
    else{
      rechit_array = new TClonesArray("PndSdsHit");
      tHits.SetBranchAddress("LMDHitsPixel",&rechit_array);  //Points for Tracks
    }
  }
  else{
    rechit_array = new TClonesArray("PndSdsHit");
    tHits.SetBranchAddress("LMDHitsStrip",&rechit_array);  //Points for Tracks
  }
  //----------------------------------------------------------------------------------
  
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  //  tgeane.SetBranchAddress("PndTrackLmd",&rec_trk);  //Tracks
  // if(kf>0){
  //   rec_trk = new TClonesArray("PndTrack");
  //   tgeane.SetBranchAddress("PndTrackLmd",&rec_trk);  //Tracks
  // }
  // else{
  //   rec_trk = new TClonesArray("PndLinTrack");
  //   tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  // }

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
  TH1 *hDiffIDs = new TH1F("hDiffIDs","Number of track-candidates with hits from diff. MC-track;N_{IDs}",10,0,10);
  TH2 *hMCtrkvshits = new TH2F("hMCtrkvshits","Number of simulated tracks vs. Number of rec. hits",
			       1000,0,1000,100,0,100);
  TH1 *hntrkcand = new TH1F("hntrkcand","Number of track-candidates per event;N_{trk-cand}",100,0,100);
  TH1 *hntrk = new TH1F("hntrk","Number of tracks per event;N_{trk}",100,0,100);
  TH1 *hntrkmissed_I = new TH1F("hntrkmissed_I","Number of missed tracks per event (#phi, #theta trks);N_{trk}",30,0,30);
  TH1 *hntrkgood_I = new TH1F("hntrkgood_I","Number of good tracks per event (#phi, #theta trks);N_{trk}",30,0,30);
  TH1 *hntrkghost_I = new TH1F("hntrkghost_I","Number of ghost tracks per event (#phi, #theta trks);N_{trk}",30,0,30);
  TH1 *hntrkmissed_II = new TH1F("hntrkmissed_II","Number of missed tracks per event (hits matching);N_{trk}",30,0,30);
  TH1 *hntrkgood_II = new TH1F("hntrkgood_II","Number of good tracks per event (hits matching);N_{trk}",30,0,30);
  TH1 *hntrkghost_II = new TH1F("hntrkghost_II","Number of ghost tracks per event (hits matching);N_{trk}",30,0,30);
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

  TH2 * hgoodPhichi2 = new TH2F("hgoodPhichi2","#phi for good track-candidates vs. #chi^{2} ",5e2,0,50,20,-355./113,355./113);
  TH2 * hgoodThetachi2 = new TH2F("hgoodThetachi2","#theta for good track-candidates vs. #chi^{2} ",5e2,0,50,20,3.5e-3,8.5e-3);
  
  //  TH1 *hMomMC = new TH1F("hMomMC","P_{MC};P,GeV/c",1e3,0,16.);
  TNtuple *nmomMC = new TNtuple("nmomMC","MCmomentum","p0:p_nearLUMI:p_afterLUMI");
  TNtuple *ntuprecTrk = new TNtuple("ntuprecTrk","Info about reconstructed trks: goodTrk [0=good,+1=ghost]","x:y:z:mom:theta:phi:goodTrk");
  TNtuple *ntupMCTrk = new TNtuple("ntupMCTrk","Info about simulated trks: goodTrk [0=good, -1=missed]","x:y:z:mom:theta:phi:goodTrk:nrechits");
  //  TH1 *hMomMCnearLUMI = new TH1F("hMomMCnear","P_{MC};P,GeV/c",1e3,0,16.);

  TH1 *hResMom = new TH1F("hResMom","P_{MC}-P_{rec};#deltaP,GeV/c",1e3,-1e-4,1e-4);
  TH1 *hErrMom = new TH1F("hErrMom","#sigma_{P};#sigmaP,GeV/c",1e3,0,1e-3);
  TH1 *hPullMom = new TH1F("hPullMom","(P_{MC}-P_{rec})/#sigma_{P};",1e3,-1e1,1e1);
  //  TH1 *hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e3,-1e-2,1e-2);
  TH1 *hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);//TEST
  TH1 *hErrTheta = new TH1F("hErrTheta","#sigma(#theta_{rec});#sigma,rad",1e3,0,0.01);
  TH1 *hPullTheta = new TH1F("hPullTheta","(#theta_{MC}-#theta_{rec})/#sigma_{#theta};",1e2,-10,10);
  TH1 *hResPhi = new TH1F("hResPhi","#phi_{MC}-#phi_{rec};#delta#phi,rad",2e3,-1.,1.);
  TH1 *hErrPhi = new TH1F("hErrPhi","#sigma(#phi_{rec});#sigma,rad",1e3,0,0.1);
  TH1 *hPullPhi = new TH1F("hPullPhi","(#phi_{MC}-#phi_{rec})/#sigma_{#phi};",1e2,-10,10);

  TH1 *hResLumiTrkMom = new TH1F("hResLumiTrkMom","P_{MC}-P_{rec}(near Lumi);#deltaP,GeV/c",1e3,-6e-7,6e-7);
  TH1 *hResLumiTrkTheta = new TH1F("hResLumiTrkTheta","#theta_{MC}-#theta_{rec}(near Lumi);#delta#theta,rad",1e3,-6e-3,6e-3);
  TH1 *hResLumiTrkPhi = new TH1F("hResLumiTrkPhi","#phi_{MC}-#phi_{rec}(near Lumi);#delta#phi,rad",2e3,-1e-1,1e-1);

  TH1 *hangMCRec = new TH1F("hangMCRec", "Angle between Pmc and Prec;#delta#alpha, rad", 1e3,0.,0.1);
  TH1 *hResPointX = new TH1F("hResPointX","X_{MC}-X_{rec};#deltaX,cm",1e2,-2.,2.);
  TH1 *hResPointY = new TH1F("hResPointY","Y_{MC}-Y_{rec};#deltaY,cm",1e2,-2.,2.);
  TH1 *hResPointZ = new TH1F("hResPointZ","Z_{MC}-Z_{rec};#deltaZ,cm",1e3,-0.15,0.15);

  TH1 *hResLumiTrkPointX = new TH1F("hResLumiTrkPointX","X_{MC}-X_{rec}(near Lumi);#deltaX,cm",1e2,-0.02,0.02);
  TH1 *hResLumiTrkPointY = new TH1F("hResLumiTrkPointY","Y_{MC}-Y_{rec}(near Lumi);#deltaY,cm",1e2,-0.02,0.02);
  TH1 *hResLumiTrkPointZ = new TH1F("hResLumiTrkPointZ","Z_{MC}-Z_{rec}(near Lumi);#deltaZ,cm",1e2,-0.02,0.02);

  TH1 *hResLumiTrkPointPx = new TH1F("hResLumiTrkPointPx","Px_{MC}-Px_{rec}(near Lumi);#deltaPx, GeV/c",1e2,-0.001,0.001);
  TH1 *hResLumiTrkPointPy = new TH1F("hResLumiTrkPointPy","Py_{MC}-Py_{rec}(near Lumi);#deltaPy, GeV/c",1e2,-0.001,0.001);
  TH1 *hResLumiTrkPointPz = new TH1F("hResLumiTrkPointPz","Pz_{MC}-Pz_{rec}(near Lumi);#deltaPz, GeV/c",1e2,-0.0001,0.0001);

  TH1 *hResLumiTrkPointXPull = new TH1F("hResLumiTrkPointXPull","(X_{MC}-X_{rec})/#sigma;(X_{MC}-X_{rec})/#sigma",1e2,-10.,10.);
  TH1 *hResLumiTrkPointYPull = new TH1F("hResLumiTrkPointYPull","(Y_{MC}-Y_{rec})/#sigma;(Y_{MC}-Y_{rec})/#sigma",1e2,-10.,10.);
  TH1 *hResLumiTrkPointZPull = new TH1F("hResLumiTrkPointZPull","(Z_{MC}-Z_{rec})/#sigma;(Z_{MC}-Z_{rec})/#sigma",1e2,-10.,10.);

  TH1 *hResLumiTrkPointPxPull = new TH1F("hResLumiTrkPointPxPull","(Px_{MC}-Px_{rec})/#sigma;(Px_{MC}-Px_{rec})/#sigma",1e2,-10,10.);
  TH1 *hResLumiTrkPointPyPull = new TH1F("hResLumiTrkPointPyPull","(Py_{MC}-Py_{rec})/#sigma;(Py_{MC}-Py_{rec})/#sigma",1e2,-10,10);
  TH1 *hResLumiTrkPointPzPull = new TH1F("hResLumiTrkPointPzPull","(Pz_{MC}-Pz_{rec})/#sigma;(Pz_{MC}-Pz_{rec})/#sigma",1e2,-10,10);

  // TH1 *hResLumiTrkTheta = new TH1F("hResLumiTrkTheta","#theta_{MC}-#theta_{rec}(near Lumi);#delta#theta, rad",1e2,-0.001,0.001);
  //  TH1 *hResLumiTrkPhi = new TH1F("hResLumiTrkPhi","#phi_{MC}-#phi_{rec}(near Lumi);#delta#phi, rad",1e2,-0.01,0.01);
  TH1 *hResLumiTrkThetaPull = new TH1F("hResLumiTrkThetaPull","(#theta_{MC}-#theta_{rec})/#sigma (near Lumi);#delta#theta, rad",1e2,-10,10);
  TH1 *hResLumiTrkPhiPull = new TH1F("hResLumiTrkPhiPull","(#phi_{MC}-#phi_{rec})/#sigma (near Lumi);#delta#phi, rad",1e2,-10,10);


  TH1 *hErrPointX = new TH1F("hErrPointX","#sigma_{X};#sigmaX,cm",1e3,0,1.);
  TH1 *hErrPointY = new TH1F("hErrPointY","#sigma_{Y};#sigmaY,cm",1e3,0,1.);
  TH1 *hErrLinPointX = new TH1F("hErrLinPointX","#sigma_{X};#sigmaX,cm",1e3,0,0.01);
  TH1 *hErrLinPointY = new TH1F("hErrLinPointY","#sigma_{Y};#sigmaY,cm",1e3,0,0.001);
  TH1 *hErrPointZ = new TH1F("hErrPointZ","#sigma_{Z};#sigmaZ,cm",1e2,0,0.01);
  TH1 *hPullPointX = new TH1F("hPullPointX","(X_{MC}-X_{rec})/#sigma_{X};(X_{MC}-X_{rec})/#sigma_{X}",1e2,-10,10);
  TH1 *hPullPointY = new TH1F("hPullPointY","(Y_{MC}-Y_{rec})/#sigma_{Y};(Y_{MC}-Y_{rec})/#sigma_{Y}",1e2,-10,10);
  TH1 *hPullPointZ = new TH1F("hPullPointZ","(Z_{MC}-Z_{rec})/#sigma_{Z};(Z_{MC}-Z_{rec})/#sigma_{Z}",1e3,-100,100);

  TH2 *hPullPointXphi = new TH2F("hPullPointXphi","(X_{MC}-X_{rec})/#sigma_{X} vs. #phi;#phi_{MC}, rad;(X_{MC}-X_{rec})/#sigma_{X}",2e1,-355./113,355./113,1e2,-10,10);
TH2 *hPullPointYphi = new TH2F("hPullPointYphi","(Y_{MC}-Y_{rec})/#sigma_{Y} vs. #phi;#phi_{MC}, rad;(Y_{MC}-Y_{rec})/#sigma_{Y}",2e1,-355./113,355./113,1e2,-10,10);
 TH2 *hPullPointXtheta = new TH2F("hPullPointXtheta","(X_{MC}-X_{rec})/#sigma_{X} vs. #theta;#theta_{MC}, rad;(X_{MC}-X_{rec})/#sigma_{X}",2e1,0.003, 0.009,1e2,-10,10);
TH2 *hPullPointYtheta = new TH2F("hPullPointYtheta","(Y_{MC}-Y_{rec})/#sigma_{Y} vs. #theta;#theta_{MC}, rad;(Y_{MC}-Y_{rec})/#sigma_{Y}",2e1,0.003, 0.009,1e2,-10,10);
  TH1 *hResPointPx = new TH1F("hResPointPx","Px_{MC}-Px_{rec};#deltaPx, GeV/c",1e2,-0.01,0.01);
  TH1 *hErrPointPx = new TH1F("hErrPointPx","#sigma_{Px};#sigmaPx, GeV/c",1e3,0,0.01);
  TH1 *hPullPointPx = new TH1F("hPullPointPx","(Px_{MC}-Px_{rec})/#sigma_{Px};(Px_{MC}-Px_{rec})/#sigma_{Px}",1e2,-10,10);
  
  TH1 *hResPointPy = new TH1F("hResPointPy","Py_{MC}-Py_{rec};#deltaPy, GeV/c",1e2,-0.01,0.01);
  TH1 *hErrPointPy = new TH1F("hErrPointPy","#sigma_{Py};#sigmaPy, GeV/c",1e3,0,0.01);
  TH1 *hPullPointPy = new TH1F("hPullPointPy","(Py_{MC}-Py_{rec})/#sigma_{Py};(Py_{MC}-Py_{rec})/#sigma_{Py}",1e2,-10,10);
  
  TH1 *hResPointPz = new TH1F("hResPointPz","Pz_{MC}-Pz_{rec};#deltaPz, GeV/c",1e2,-1e-3,1e-3);
  TH1 *hErrPointPz = new TH1F("hErrPointPz","#sigma_{Pz};#sigmaPz, GeV/c",1e3,0,1e-1);
  TH1 *hPullPointPz = new TH1F("hPullPointPz","(Pz_{MC}-Pz_{rec})/#sigma_{Pz};(Pz_{MC}-Pz_{rec})/#sigma_{Pz}",1e2,-10,10);
  
  TH1 *hResDCA = new TH1F("hDCA","|DCA|;|DCA|,cm",1e3,0,10.);
  TH1 *hErrDCA = new TH1F("hErrDCA","#sigma_{DCA};#sigmaDCA,cm",1e3,0,1.);
  TH1 *hPullDCA = new TH1F("hPullDCA","|DCA|/#sigma_{DCA};",1e2,0,10);
  
  TH2 *hPointXY = new TH2F("hPointXY","PCA;X,cm;Y,cm",1e3,-10,10.,1e3,-10,10.);
  TH2 *hPointXYcut = new TH2F("hPointXYcut","PCA;#X,cm;#Y,cm",1e3,-1.,1.,1e3,-1.,1.);
  TH2 *hErrPointXY = new TH2F("hErrPointXY","#sigma_{PCA};#sigmaX,cm;#sigmaY,cm",1e3,0,30.,1e3,0,30.);
  
  TH2 *hHitIDHitX = new TH2F("hHitIDHitX",";MC_{ID} ;hit_{X}, cm;",6,-1,5,1e3,24.,45.);
  TH2 *hHitIDHitY = new TH2F("hHitIDHitY",";MC_{ID} ;hit_{Y}, cm;",6,-1,5,1e3,-10.,10.);
  
  TH2 *hRecGEANEX = new TH2F("hRecGEANEX",";X_{rec}, cm;X_{GEANE}, cm",1e3,-15.,15.,1e3,-100,100.);
  TH2 *hRecGEANEY = new TH2F("hRecGEANEY",";Y_{rec}, cm;Y_{GEANE}, cm",1e3,-15.,15.,1e3,-100,100.);
  TH2 *hRecGEANEZ = new TH2F("hRecGEANEZ",";Z_{rec}, cm;Z_{GEANE}, cm",1e3,-15.,15.,1e3,-1.,1.);
  TH2 *hRecGEANER = new TH2F("hRecGEANER",";R_{rec}, cm;R_{GEANE}, cm",1e3,0,10.,1e3,0,10.);
  TH2 *hRecGEANETheta = new TH2F("hRecGEANETheta",";#theta_{rec}, rad;#theta_{GEANE}, rad",1e3,0,1,1e3,0,1);
  TH2 *hRecGEANEPhi = new TH2F("hRecGEANEPhi",";#phi_{rec}, rad;#phi_{GEANE}, rad",1e3,-7,7,1e3,-7,7);
  TH2 *hRecThetaPhi = new TH2F("hRecThetaPhi",";#theta_{rec}, rad;#phi_{rec}, rad",1e3,0,1.,1e3,-7,7);
  
  TH2 *hSeedGEANEX = new TH2F("hSeedGEANEX",";X_{cand}, cm;X_{GEANE}, cm",1e3,-100.,100.,1e3,-100.,100.);
  TH2 *hSeedGEANEY = new TH2F("hSeedGEANEY",";Y_{cand}, cm;Y_{GEANE}, cm",1e3,-100.,100.,1e3,-100.,100.);
  TH2 *hSeedGEANEZ = new TH2F("hSeedGEANEZ",";Z_{cand}, cm;Z_{GEANE}, cm",1e3,-0.015,0.015,1e3,-0.05,0.05);
  TH2 *hSeedGEANER = new TH2F("hSeedGEANER",";R_{cand}, cm;R_{GEANE}, cm",1e3,0,100.,1e3,0,100.);
  TH2 *hSeedGEANETheta = new TH2F("hSeedGEANETheta",";#theta_{cand}, rad;#theta_{GEANE}, rad",1e3,0,0.1,1e3,0,0.1);
  TH2 *hSeedGEANEPhi = new TH2F("hSeedGEANEPhi",";#phi_{cand}, rad;#phi_{GEANE}, rad",1e3,-7,7,1e3,-7,7);
  TH2 *hSeedThetaPhi = new TH2F("hSeedThetaPhi",";#theta_{cand}, rad;#phi_{cand}, rad",1e3,0,1.,1e3,-1.,1.);
  
  TH2 *hSeedThetaDiffIDs = new TH2F("hSeedThetaDiffIDs",";# IDs;#theta_{cand}, rad",7,0,7,1e3,0,0.1);
  TH2 *hSeedPhiDiffIDs = new TH2F("hSeedPhiDiffIDs",";# IDs;#phi_{cand}, rad",7,0,7,1e3,-4.,4.);
  TH2 *hDiffIDsPointX = new TH2F("hDiffIDsPointX",";# IDs;X,cm",7,0,7,1e3,-10,10.);
  TH2 *hDiffIDsPointY = new TH2F("hDiffIDsPointY",";# IDs;Y,cm",7,0,7,1e3,-10,10.);
  TH2 *hDiffIDsPointZ = new TH2F("hDiffIDsPointZ",";# IDs;Z,cm",7,0,7,1e3,-1.,1.);
  
  TH2 *hMCThetaGEANETheta = new TH2F("hMCThetaGEANETheta",";#theta_{MC}, rad;#theta_{GEANE}, rad",1e3,0,0.012,1e3,0,0.012);
  TH2 *hMCPhiGEANEPhi = new TH2F("hMCPhiGEANEPhi",";#phi_{MC}, rad;#phi_{GEANE}, rad",1e3,-3.15,3.15,1e3,-3.15,3.15);
  
  TH2 *hMCThetaResTheta = new TH2F("hMCThetaResTheta",";#theta_{MC}, rad;#Delta#theta, rad",1e2,0,0.012,1e3,-1e-2,1e-2);
  TH2 *hMCPhiResPhi = new TH2F("hMCPhiResPhi",";#phi_{MC}, rad;#Delta#phi, rad",1e2,-3.15,3.15,1e2,-5e-1,5e-1);
  TH2 *hMCThetaResSeedTheta = new TH2F("hMCThetaResSeedTheta",";#theta_{MC}, rad;#Delta#theta, rad",1e2,0.,0.1,1e3,-1e-2,1e-2);
  TH2 *hMCPhiResSeedPhi = new TH2F("hMCPhiResSeedPhi",";#phi_{MC}, rad;#Delta#phi, rad",1e2,-3.15,3.15,1e2,-1e-1,1e-1);
  TH2 *hResZResPhi = new TH2F("hResZResPhi",";#Delta#phi, rad;#Delta Z, cm",1e3,-2e-1,2e-1,1e3,-0.02,0.02);
  TH2 *hntrkMCtrkID = new TH2F("hntrkMCtrkID","Number of participation this MCid in rec.tracks;ID;Number of rec.trks",10,0,10,10,0,10);
  TH2 *hchi2MCdiffID = new TH2F("hchi2MCdiffID","number diff MCid in rec.tracks vs. #chi^{2};#chi^{2};N",5e2,0,50,10,0,10);

  ///params of lin trk in lumi
  TH1 *hLumiTrkA = new TH1F("hLumiTrkA","param a [x=a*z+b];",1e3,-0.1,0.1);
  TH1 *hLumiTrkB = new TH1F("hLumiTrkB","param b [x=a*z+b];",1e3,-10.,10.);
  TH1 *hLumiTrkC = new TH1F("hLumiTrkC","param c [y=c*z+d];",1e3,-0.1,0.1);
  TH1 *hLumiTrkD = new TH1F("hLumiTrkD","param d [y=c*z+d];",1e3,-10.,10.);

  //-----------------------------------------------------------------------------
  
  TH2 *hchi2Errx = new TH2F("hchi2Errx","#sigma_{X} vs. #chi^2 for reconstructed tracks;#chi^2;#sigma_{X}, cm",
			    1e3,0,100,2e1,0,0.2);
  TH2 *hchi2Erry = new TH2F("hchi2Erry","#sigma_{Y} vs. #chi^2 for reconstructed tracks;#chi^2;#sigma_{X}, cm",
			    1e3,0,100,2e1,0,0.2);
 TH2 *hchi2nTrkCand = new TH2F("hchi2nTrkCand"," ;Number of trk-cand;#chi^2",
			    30,0,30,5e2,0,50.);
  TNtuple *nrecpointall = new TNtuple("nrecpointall","recpointAll","xrecbp:yrecbp:zrecbp:xrec:yrec:zrec:xseed:yseed:zseed:chi2");
  TNtuple *nrecdirall = new TNtuple("nrecdirall","recdirAll","dxmc:dymc:dzmc:dxrecbp:dyrecbp:dzrecbp:dirxrec:diryrec:dirzrec:dirxseed:diryseed:dirzseed");
  TNtuple *nsectors = new TNtuple("nsectors","sectors","thetares:sector");
  TH2I *hnhits = new TH2I("hnhits","# rec hits vs. # sim hits; sim; rec",100,0,100,100,0,100);
  //Load lumi geo params
  PndLmdDim *lmddim = PndLmdDim::Instance();
  // lmddim -> Read_transformation_matrices("matrices.txt", true);
  lmddim -> Read_transformation_matrices("matrices_perfect.txt", false);
  int glBADGEANE=0;
  int glBadEv = 0;
  int glNoisehit = 0;// total number of noise hits
  for (Int_t j=0; j<nEvents; j++){
    //  cout<<"Event #"<<j<<endl;
    // Read GEANE & MC info -----------------------------------------------------------------
    //    if(kf<1) 
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
      cout<<"Event #"<<j<<" has "<<nParticles<<" true particles, "<<" out of it "<<nRecHits<<" hits, "<<nTrkCandidates
	  <<" trk-cands, "<<numTrk<<" tracks and "<<nGeaneTrks<<" geane Trks!"<<endl;
    hnRecnMC->Fill(nParticles,nGeaneTrks);
    if(nParticles!=nMCtracks) continue;
    hnhits->Fill(nMCHits,nRecHits);
    if(nRecHits<3*nMCtracks) glBadEv++;
    // if(nRecHits<3*nMCtracks) cout<<"Event #"<<j<<" doesn't have enough rec.hits!!!"<<endl;
    // if(nRecHits<3*nMCtracks) continue;
    double chi2Cont[5*numTrk];
    double ndiffIDCont[5*numTrk];
   
    hMCtrkvshits->Fill(nRecHits,nParticles);
    if(nTrkCandidates>numTrk) cout<<"Event #"<<j<<" has "<<nTrkCandidates<<" trk-cands and "<<numTrk<<" tracks!"<<endl;
   
    hntrkcand->Fill(nTrkCandidates);
    hntrkcandvsMC->Fill(nParticles,nTrkCandidates);
    
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

    
      vector<int> MCtrkID; //arrray of hits MCid
      Int_t diffIDs=1;
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(iN);
      ///get rid from most probably ghost track ------------
      TVector3 PosRec = fRes->GetPosition();
      double pca_lim = 1.;//=10*sigma_Xpca~10*{0.093,0.11,0.12,0.22,0.55};
      if(Plab<5) pca_lim = 2.;
      if(Plab<2) pca_lim = 5.;
      // if(fabs(PosRec.X())>pca_lim && fabs(PosRec.Y())>pca_lim){
      // 	cout<<"666 Event #"<<j<<" has too large X_pca ot Y_pca 666"<<endl;
      // }
      if(fabs(PosRec.X())>pca_lim && fabs(PosRec.Y())>pca_lim) continue; // PCA_x and PCA_y should be < 10sigmaX

     
      ///get rid from most probably ghost track (END) ---
      Double_t lyambda = fRes->GetLambda();
      if(lyambda==0){
	cout<<"GEANE didn't propagate this trk!"<<endl;
	cout<<"Event #"<<j<<" diffIDs = "<<diffIDs<<endl;
	glBADGEANE++;
      }
      if(lyambda==0) continue;
      PndLinTrack *trk;
      double linpar[6];
      Double_t errparlin[6];
      double chi2;
      PndTrack *trkpnd;
      Int_t candID;
      PndTrackCand *trkcand;
      Int_t trkSensor;
	trk = (PndLinTrack*)rec_trk->At(iN);
	trk->GetPar(linpar);
	hLumiTrkA->Fill(linpar[1]); 
	hLumiTrkB->Fill(linpar[0]); 
	hLumiTrkC->Fill(linpar[3]); 
	hLumiTrkD->Fill(linpar[2]); 
	trk->GetParErr(errparlin);
	candID = trk->GetTCandID();
	trkcand = (PndTrackCand*)trkcand_array->At(candID);
	chi2 = trk->GetChiSquare();
	chi2Cont[iN] = chi2;
	hchi2nTrkCand->Fill(nTrkCandidates,chi2);
      
	// PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(iN); //TODO: find out how to check trk-cand in case then cut are applied to trk-fit results
	const int Ntrkcandhits= trkcand->GetNHits();
	PndSdsMCPoint* MCPointHit;
	
	//Matching between MC & Rec on hits level-----------------------------------
	if(verboseLevel>1) cout<<"MCidTOP:"<<endl;
	double momMC0,momMC1,momMC2;
	momMC0 = Plab;
	for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){ // loop over rec.hits
	  PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	Int_t hitID = candhit.GetHitId();
	PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));

	//for pixel design
	double MCpointMom;
	if(dnu>0){
	  //  if(verboseLevel>1) cout<<"Rec hit("<<myHit->GetClusterIndex()<<")";
	  PndSdsClusterPixel* myCluster = (PndSdsClusterPixel*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	  PndSdsDigiPixel* astripdigi = (PndSdsDigiPixel*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	  if (astripdigi->GetIndex(0) == -1){
	    glNoisehit++;
	    //  MCtrkID.push_back(-111);
	    continue;
	  }
	int sensorID = myHit->GetSensorID();
	int ihalf, iplane, imodule, iside, idie, isensor;
	// calculate the plane and sensor on this plane
	lmddim->Get_sensor_by_id(sensorID, ihalf, iplane, imodule, iside, idie, isensor);
	trkSensor = ihalf*5+imodule;
	  // if (astripdigi->GetIndex(0) == -1) continue; // sort out noise
	  PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	  MCpointMom = sqrt(MCPoint->GetPx()*MCPoint->GetPx()+MCPoint->GetPy()*MCPoint->GetPy()+MCPoint->GetPz()*MCPoint->GetPz());
	  int MCidTOP = MCPoint->GetTrackID();
	  if(iHit==0) MCPointHit = MCPoint;
	  MCtrkID.push_back(MCidTOP);
	  //	  if(verboseLevel>1) cout<<"MCid("<<MCidTOP<<") ";
	  if(verboseLevel>1) cout<<MCidTOP<<" ";
	}

	//for strip design
	else{
	///Top cluster
	PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	if (astripdigi->GetIndex(0) == -1) glNoisehit++;
	if (astripdigi->GetIndex(0) == -1) continue; // sort out noise
	PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	MCpointMom = sqrt(MCPoint->GetPx()*MCPoint->GetPx()+MCPoint->GetPy()*MCPoint->GetPy()+MCPoint->GetPz()*MCPoint->GetPz());

	int MCidTOP = MCPoint->GetTrackID();
	if(verboseLevel>1) cout<<MCidTOP<<" ";
	if(iHit==0) MCPointHit = MCPoint;
	// double zTop = MCPoint->GetZ();
	// double xTop = MCPoint->GetX();
	// double yTop = MCPoint->GetY();
	///Bottom cluster
	Int_t  botIndex = myHit->GetBotIndex();
	PndSdsClusterStrip* myClusterBot =  (PndSdsClusterStrip*)(fStripClusterArray->At(botIndex));
	PndSdsDigiStrip* astripdigiBot = (PndSdsDigiStrip*)(fStripDigiArray->At(myClusterBot->GetDigiIndex(0)));
	if (astripdigiBot->GetIndex(0) == -1) glNoisehit++;
	if (astripdigiBot->GetIndex(0) == -1) continue; // sort out noise
	PndSdsMCPoint* MCPointBot = (PndSdsMCPoint*)(true_points->At(astripdigiBot->GetIndex(0)));
	// double zBot = MCPointBot->GetZ();
	// double xBot = MCPointBot->GetX();
	// double yBot = MCPointBot->GetY();
	// cout<<"zTop - zBot = "<<zTop-zBot<<endl;
	// cout<<"xTop - xBot = "<<xTop-xBot<<endl;
	// cout<<"yTop - yBot = "<<yTop-yBot<<endl;
	int MCidBOT = MCPointBot->GetTrackID();
	//	if(MCidTOP!=MCidBOT) continue;
	MCtrkID.push_back(MCidTOP);
	MCtrkID.push_back(MCidBOT);
	}
	if(iHit==0) momMC1 = MCpointMom;
	if(iHit==(Ntrkcandhits-1)) momMC2 = MCpointMom;
      }

      // TNtuple *nmomMC = new TNtuple("nmomMC","MCmomentum","p0:p_nearLUMI:p_afterLUMI");
      nmomMC->Fill(momMC0,momMC1,momMC2);
      if(verboseLevel>1) cout<<""<<endl;
      //   Sorting MC IDs ---------------------------------------- 
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
      ///--------------------------------------------------------------------------
      
      /// Counting number of diff MC ids ----------------------------------------      
      Int_t prevID = MCtrkID[0];
      for(int nk=0;nk<nParticles;nk++){
	if(MCtrkID[0]==nk)
	  MCtrk[nk]=MCtrk[nk]+1;
      }
      diffIDs = 1;
      for(Int_t n=1; n<MCtrkID.size(); n++){ 
	if(prevID<MCtrkID[n]){
	  diffIDs++;
	  prevID=MCtrkID[n];
	  for(int nk=0;nk<nParticles;nk++){
	    if(MCtrkID[n]==nk)
	      MCtrk[nk]=MCtrk[nk]+1;
	  }
	}
      }
    
      ndiffIDCont[iN]=diffIDs;
      hDiffIDs->Fill(diffIDs);
      hntrkcandvsIDs->Fill(diffIDs,nTrkCandidates);
      hnhitsvsIDs->Fill(diffIDs,Ntrkcandhits);
      ///------------------------------------------------------------------------

      /// Set track quality: good or ghost? and assign MC id to rec trk ---------
      if(diffIDs<2){
	RECtrkMCid[iN] = MCtrkID[0];
	goodTrk[iN] = true;
      }
      else{
	vector<int> countMC_IDs(diffIDs);
	prevID = MCtrkID[0];
	int diffCount=0;
	for(Int_t n=0; n<MCtrkID.size(); n++) {
	  countMC_IDs[diffCount]++;
	  if(prevID<MCtrkID[n]){
	    diffCount++;
	    prevID=MCtrkID[n];
	  }
	}
	int maxID=countMC_IDs[0];
	int posIDmax=0;
	for(int kn=0;kn<diffIDs;kn++){
	  //	  cout<<"countMC_IDs["<<kn<<"]="<<countMC_IDs[kn]<<" 0.7*MCtrkID.size() = "<<0.7*MCtrkID.size()<<endl;
	  if(countMC_IDs[kn]>0.65*MCtrkID.size()){ //more then 65% of hits come from the same MC id
	    goodTrk[iN] = true;
	    ghostTrk[iN] = false;
	  }
	  else{
	    if(!goodTrk[iN]) ghostTrk[iN] = true;
	  }
	  
	  if(countMC_IDs[kn]>maxID){ 
	    maxID=countMC_IDs[kn];
	    posIDmax = kn;
	  }
	}
	prevID = MCtrkID[0];
	diffCount=0;
	for(Int_t n=0; n<MCtrkID.size(); n++) {
	  if(diffCount==posIDmax) RECtrkMCid[iN] = prevID;
	  if(prevID<MCtrkID[n]){
	    diffCount++;
	    prevID=MCtrkID[n];
	  }
	}
      }
      ///------------------------------------------------------------------------
      
      /// Comporision MC, trk-candidates and REConstructed tracks --------------------------------------------
      TVector3 posSeed = trkcand->getPosSeed();
      TVector3 dirSeed = trkcand->getDirSeed();
      hSeedThetaDiffIDs->Fill(diffIDs,dirSeed.Theta());
      hSeedPhiDiffIDs->Fill(diffIDs,dirSeed.Phi()); 
      if(diffIDs>-1){ // All tracks
	/// Try to find connection between position of -----------
	/// reconstructed track and track after GEANE propagation
	TVector3 pos_prop_geane_trk = fRes->GetPosition();
	Double_t theta_prop_geane_trk = TMath::Pi()/2. - lyambda;
	Double_t phi_prop_geane_trk = fRes->GetPhi();
	TVector3 pos_rec_trk,dir_rec_trk;

	  pos_rec_trk = trk->GetStartVec();
	  dir_rec_trk = trk->GetDirectionVec();
	  hRecGEANEX->Fill(pos_rec_trk.X(),pos_prop_geane_trk.X());
	  hRecGEANEY->Fill(pos_rec_trk.Y(),pos_prop_geane_trk.Y());
	  hRecGEANEZ->Fill(pos_rec_trk.Z(),pos_prop_geane_trk.Z());
	  hRecGEANER->Fill(pos_rec_trk.Perp(),pos_prop_geane_trk.Perp());
	  hRecGEANETheta->Fill(dir_rec_trk.Theta(), theta_prop_geane_trk);
	  hRecGEANEPhi->Fill(dir_rec_trk.Phi(), phi_prop_geane_trk);
	  hRecThetaPhi->Fill(dir_rec_trk.Theta(),dir_rec_trk.Phi());
	  
	  hSeedGEANEX->Fill(posSeed.X(),pos_prop_geane_trk.X());
	  hSeedGEANEY->Fill(posSeed.Y(),pos_prop_geane_trk.Y());
	  hSeedGEANEZ->Fill(posSeed.Z(),pos_prop_geane_trk.Z());
	  hSeedGEANER->Fill(posSeed.Perp(),pos_prop_geane_trk.Perp());
	  hSeedGEANETheta->Fill(dirSeed.Theta(), theta_prop_geane_trk);
	  hSeedGEANEPhi->Fill(dirSeed.Phi(), phi_prop_geane_trk);
	  hSeedThetaPhi->Fill(dirSeed.Theta(),dir_rec_trk.Phi());

	///-------------------------------------------------------
	
	/// CUT: Check position and coordinates errors of PCA ---------------------------------------

	hDiffIDsPointX->Fill(diffIDs,PosRec.X());
	hDiffIDsPointY->Fill(diffIDs,PosRec.Y());
	hDiffIDsPointZ->Fill(diffIDs,PosRec.Z());
	Double_t CovGEANELAB[6][6];
	fRes->GetMARSCov(CovGEANELAB);
	
	Double_t errX = fRes->GetDX();
	Double_t errY = fRes->GetDY();
	Double_t errZ = fRes->GetDZ();
	
	Double_t errXlin = errparlin[0];
	Double_t errYlin = errparlin[1];
	hchi2Errx->Fill(chi2,errX);
	hchi2Erry->Fill(chi2,errY);
	nRecGEANEtrk++;
	///-----------------------------------------------------------------------------------
	
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

	double resMom = MomRecBP.Mag()-MomMC.Mag();
	hgoodTheta->Fill(thetaMC);
	hgoodPhi->Fill(phiMC);
	hgoodPhiTheta->Fill(thetaMC,phiMC);
	hgoodThetachi2->Fill(chi2,thetaMC);
	hgoodPhichi2->Fill(chi2,phiMC);
	
	hMCThetaGEANETheta->Fill(thetaMC,thetaBP);
	hMCPhiGEANEPhi->Fill(phiMC,phiBP);
	hMCThetaResTheta->Fill(thetaMC,thetaMC-thetaBP);
	hMCPhiResPhi->Fill(phiMC,phiMC-phiBP);
	hResPointX->Fill(PosRec.X());
	hResPointY->Fill(PosRec.Y());
	hResDCA->Fill(PosRec.Mag());
	Double_t errPCA = TMath::Sqrt(errX*errX+errY*errY+errZ*errZ);
	hErrDCA->Fill(errPCA);
	hPullDCA->Fill(PosRec.Mag()/errPCA);
	hPointXY->Fill(-PosRec.X(), -PosRec.Y());
	hPointXYcut->Fill(-PosRec.X(), -PosRec.Y());
	hResPointZ->Fill(-PosRec.Z());
	hErrPointX->Fill(errX);
	hErrPointY->Fill(errY);
	hErrLinPointX->Fill(errXlin);
	hErrLinPointY->Fill(errYlin);
	hErrPointXY->Fill(errX,errY);
	hErrPointZ->Fill(errZ);
	hPullPointX->Fill(-PosRec.X()/errX);
	hPullPointY->Fill(-PosRec.Y()/errY);
	hPullPointZ->Fill(-PosRec.Z()/errZ);
	hPullPointXphi->Fill(phiMC,(-PosRec.X()/errX));
	hPullPointYphi->Fill(phiMC,(-PosRec.Y()/errY));
	hPullPointXtheta->Fill(thetaMC,(-PosRec.X()/errX));
	hPullPointYtheta->Fill(thetaMC,(-PosRec.Y()/errY));

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
	nsectors->Fill(resTheta,trkSensor);
	hErrTheta->Fill(err_lyambda);
	hPullTheta->Fill(resTheta/err_lyambda);
	hResPhi->Fill(resPhi);
	hErrPhi->Fill(err_phi);
	hPullPhi->Fill(resPhi/err_phi);
	hResZResPhi->Fill(resPhi,PosRec.Z());
	
	Double_t angMCRec = MomMC.Angle(MomRecBP); // Angle between two vectors
	hangMCRec->Fill(angMCRec);
	hchi2->Fill(chi2);

	///Let's checked for good tracks by phi\theta definition ------------------

	for(Int_t jk=0; jk< nParticles;jk++){
	  PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(jk);
	  Int_t mcID = mctrk->GetPdgCode();
	  if(mcID==-2212){
	    TVector3 MomMC = mctrk->GetMomentum();
	    double diffTheta=fabs(MomMC.Theta()-thetaBP)/err_lyambda;
	    double diffPhi=fabs(MomMC.Phi()-phiBP)/err_phi;

	    if(fabs(MomMC.Phi())<0.14 || fabs(MomMC.Phi())>3){ //for #phi near edge reconstructed angle can differ
	      double phiMC1 = -MomMC.Phi();
	      double diffPhi2=fabs(phiMC1-phiBP)/err_phi;
	      if(diffPhi2<diffPhi) diffPhi = diffPhi2;
	    }
	
	    hntrkmissedPhiTheta->Fill(diffTheta,diffPhi);
	    if(diffTheta<4. && diffPhi<4.) goodRectrk++;
	  }
	}
	///(end) check for good tracks --------------------------
	nrecpointall->Fill(pos_prop_geane_trk.X(),pos_prop_geane_trk.Y(),pos_prop_geane_trk.Z(),
			   pos_rec_trk.X(),pos_rec_trk.Y(),pos_rec_trk.Z(),
			   posSeed.X(),posSeed.Y(),posSeed.Z(),chi2);
	nrecdirall->Fill(MomMC.X()/MomMC.Mag(),MomMC.Y()/MomMC.Mag(),MomMC.Z()/MomMC.Mag(),MomRecBP.X()/MomRecBP.Mag(),MomRecBP.Y()/MomRecBP.Mag(),MomRecBP.Z()/MomRecBP.Mag(),
			 dir_rec_trk.X(),dir_rec_trk.Y(),dir_rec_trk.Z(),
			 dirSeed.X(),dirSeed.Y(),dirSeed.Z());
	///==================================
	
	
	///Compare lin trk and MC trk in LUMI frame========================
	// cout<<" "<<endl;
	// cout<<"=============================="<<endl;
	TVector3 vtxLumi = trk->GetStartVec();
	TVector3 vtxLumiErr = trk->GetStartErrVec();
	TVector3 dirLumi = trk->GetDirectionVec();
	TVector3 dirLumiErr = trk->GetDirectionErrVec();
	// // //do the transformation from LUMI frame (with z-axis perp. to lumi planes) to lab frame
	// if(dnu>0){
	//   vtxLumi = lmddim->Transform_lmd_local_to_global(vtxLumi, false, false);
	//   dirLumi = lmddim->Transform_lmd_local_to_global(dirLumi, true, false);
	//   vtxLumiErr = lmddim->Transform_lmd_local_to_global(vtxLumiErr, true, false);
	//   dirLumiErr = lmddim->Transform_lmd_local_to_global(dirLumiErr, true, false);
	// } 
	// else{
	//   combitransFromLumiFrame(vtxLumi,dnu);
	//   rotateFromLumiFrame(dirLumi, false, dnu);
	//   rotateFromLumiFrame(dirLumiErr, true, dnu);
	//   rotateFromLumiFrame(vtxLumiErr, true, dnu);	
	// }
	double xTrue = MCPointHit->GetX();
	double yTrue = MCPointHit->GetY();
	double zTrue = MCPointHit->GetZ();
	TVector3 vtxLumiMC = TVector3(xTrue,yTrue,zTrue);
	
	
	double pxTrue =  MCPointHit->GetPx();
	double pyTrue =  MCPointHit->GetPy();
	double pzTrue =  MCPointHit->GetPz();
	TVector3 dirLumiMC = TVector3(pxTrue,pyTrue,pzTrue);
	//	hMomMC->Fill(dirLumiMC.Mag());
	dirLumiMC *=1./dirLumiMC.Mag();
	
	double dz = -zTrue+vtxLumi.Z();//Correct definition Z coord for comparision
	//	double dz = 0;//TEST
	double dx = dirLumiMC.X()*dz;
	double dy = dirLumiMC.Y()*dz;
	vtxLumiMC += TVector3(dx,dy,dz);
	
	hResLumiTrkMom->Fill(dirLumiMC.Mag()-dirLumi.Mag());
	hResLumiTrkTheta->Fill(dirLumiMC.Theta()-dirLumi.Theta());
	hResLumiTrkPhi->Fill(dirLumiMC.Phi()-dirLumi.Phi());
	hResLumiTrkThetaPull->Fill((dirLumiMC.Theta()-dirLumi.Theta())/dirLumiErr.Theta());
	//	hResLumiTrkPhiPull->Fill(dirLumiMC.Phi()-dirLumi.Phi());
	hResLumiTrkPhiPull->Fill((dirLumiMC.Phi()-dirLumi.Phi())/dirLumiErr.Phi());

	hResLumiTrkPointX->Fill(vtxLumiMC.X()-vtxLumi.X());
	hResLumiTrkPointY->Fill(vtxLumiMC.Y()-vtxLumi.Y());
	hResLumiTrkPointZ->Fill(vtxLumiMC.Z()-vtxLumi.Z());
	
	hResLumiTrkPointPx->Fill(dirLumiMC.X()-dirLumi.X());
	hResLumiTrkPointPy->Fill(dirLumiMC.Y()-dirLumi.Y());
	hResLumiTrkPointPz->Fill(dirLumiMC.Z()-dirLumi.Z());
	
	hResLumiTrkPointXPull->Fill((vtxLumiMC.X()-vtxLumi.X())/vtxLumiErr.X());
	hResLumiTrkPointYPull->Fill((vtxLumiMC.Y()-vtxLumi.Y())/vtxLumiErr.Y());
	hResLumiTrkPointZPull->Fill((vtxLumiMC.Z()-vtxLumi.Z())/vtxLumiErr.Z());
	
	hResLumiTrkPointPxPull->Fill((dirLumiMC.X()-dirLumi.X())/dirLumiErr.X());
	hResLumiTrkPointPyPull->Fill((dirLumiMC.Y()-dirLumi.Y())/dirLumiErr.Y());
	hResLumiTrkPointPzPull->Fill((dirLumiMC.Z()-dirLumi.Z())/dirLumiErr.Z());
	
	hMCThetaResSeedTheta->Fill(dirLumiMC.Theta(),(dirLumiMC.Theta()-dirLumi.Theta()));
	hMCPhiResSeedPhi->Fill(dirLumiMC.Phi(),(dirLumiMC.Phi()-dirLumi.Phi()));
	///================================================================
      }
      ///-------------------------------------------------------------------------------------
      MCtrkID.clear();
    }
    for(int nk=0;nk<numTrk;nk++){
      hntrkMCtrkID->Fill(nk,MCtrk[nk]);
      hchi2MCdiffID->Fill(chi2Cont[nk],ndiffIDCont[nk]);
    }
    /// (I) missed\ghost tracks are defined on Phi\Theta difference between MC&REC trks
    hntrkgood_I->Fill(goodRectrk);
    if((nMCtracks-goodRectrk)>0) hntrkmissed_I->Fill(nMCtracks-goodRectrk);
    if((nGeaneTrks-goodRectrk)>0) hntrkghost_I->Fill(nGeaneTrks-goodRectrk);
    if(verboseLevel>0){
    cout<<nMCtracks<<" trks per event were simulated and "<<nGeaneTrks<<" were reconstructed"<<endl;
    cout<<"-- (I) dPhi\dTheta -------------------------"<<endl;
    cout<<"Good trks: "<<goodRectrk<<" missed: "<<nMCtracks-goodRectrk<<" ghost: "<<nGeaneTrks-goodRectrk<<endl;
    cout<<"--------------------------------"<<endl;
    }
    /// (II) missed\ghost tracks are defined on hits information
    int goodRecII=0, ghostRecII=0;
    for (Int_t iN=0; iN<nGeaneTrks; iN++){
      int trkType = -1;
      if(goodTrk[iN]){
	goodRecII++;
	trkType = 0;
      }
      if(ghostTrk[iN]){
	ghostRecII++;
	trkType = +1;
      }

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
    if((nMCtracks-goodRecII)>0){ 
      hntrkmissed_II->Fill(nMCtracks-goodRecII);
    }
    
    for(int imc=0;imc<nParticles;imc++){//MC trks
      bool missTrk=true;
      for(int irec=0;irec<nGeaneTrks;irec++){//RECids assigment
	int mc_comp = RECtrkMCid[irec];
	if(mc_comp==imc) missTrk=false;
      }
      
      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(imc);
      TVector3 MomMC = mctrk->GetMomentum();
      TVector3 PosMC = mctrk->GetStartVertex();
      int trkQ;
      if(missTrk) trkQ=-1;
      else trkQ=0;
      ntupMCTrk->Fill(PosMC.X(),PosMC.Y(),PosMC.Z(),MomMC.Mag(),MomMC.Theta(),MomMC.Phi(),trkQ,nRecHits);
    }

    if(verboseLevel>0){
      cout<<"-- (II) Hits matching -------------------------"<<endl;
      cout<<"Good trks: "<<goodRecII<<" missed: "<<nMCtracks-goodRecII<<" ghost: "<<ghostRecII<<endl;
      cout<<"--------------------------------"<<endl;
      cout<<" "<<endl;
    }
    //  cout<<"number of good: "<<goodRecII<<" number of missed: "<<nMCtracks-goodRecII<<" number of ghost: "<<ghostRecII<<endl;
    ///-------------------------------------------------------------------------------------
  
    //    cout<<"nGeaneTrks = "<<nGeaneTrks<<" nRecGEANEtrk = "<<nRecGEANEtrk<<endl;
    hntrk->Fill(nRecGEANEtrk);
    ///-------------------------------------------------------------------------------------------
    //  cout<<"========="<<endl;
    for(Int_t jk=0; jk< nParticles;jk++){
      // if(nParticles!=4) continue;
      PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(jk);
      Int_t mcID = mctrk->GetPdgCode();
      if(mcID==-2212){
	//  if(nParticles!=4) cout<<"For event #"<<j<<" mcID="<<mcID<<endl;
	TVector3 MomMC = mctrk->GetMomentum();
	hallTheta->Fill(MomMC.Theta());
	hallPhi->Fill(MomMC.Phi());
	hallPhiTheta->Fill(MomMC.Theta(),MomMC.Phi());
      }
    }
  }
  
  // TCanvas *c1 = new TCanvas("IDsINFO");
  // c1->Divide(2,2);
  // c1->cd(1);
  // hnRecnMC->Draw();
  // c1->cd(2);
  // hntrkcand->Draw();
  // hntrk->SetLineColor(2);
  // hntrk->Draw("same");
  // c1->cd(3);
  // hDiffIDs->Draw();
  // c1->cd(4);
  // // hntrkcandvsIDs->Draw();
  // hchi2->Draw();
  // c1->Write();
  // c1->Close();
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
  //  hntrkcandvsIDs->Draw("colz");
  // hntrkcandvsMC->Draw("colz");
  hnhitsvsIDs->Draw("colz");
  //hchi2->Draw();
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
  //  hPullMom->Fit(funp,"r");
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



  TCanvas *c41 = new TCanvas("ResLinINFO");
  c41->Divide(2,2);
  c41->cd(1);
  hErrLinPointX->Draw();
  c41->cd(2);
  hErrLinPointY->Draw();
  c41->Write();
  c41->Close();

  // TCanvas *c5 = new TCanvas("HitIdINFO");
  // c5->Divide(1,2);
  // c5->cd(1);
  // hHitIDHitX->SetMarkerStyle(24);
  // hHitIDHitX->Draw();
  // c5->cd(2);
  // hHitIDHitY->SetMarkerStyle(24);
  // hHitIDHitY->Draw();
  // c5->Write();
  // c5->Close();
 
  TCanvas *c6 = new TCanvas("VertexINFO");
  c6->Divide(2,2);
  c6->cd(1);
  hPointXY->Draw();
  c6->cd(2);
  hPointXYcut->Draw();
  c6->cd(3);
  hErrPointXY->Draw();
  c6->Write();
  c6->Close();
  
  TCanvas *c7 = new TCanvas("GEANEvsRecINFO");
  c7->Divide(3,2);
  c7->cd(1);
  hRecGEANEX->Draw();
  c7->cd(2);
  hRecGEANEY->Draw();
  c7->cd(3);
  hRecGEANEZ->Draw();
  c7->cd(4);
  hRecGEANETheta->Draw();
  c7->cd(5);
  hRecGEANEPhi->Draw();
  c7->cd(6);
  hRecThetaPhi->Draw();
  c7->Write();
  c7->Close();

  TCanvas *c8 = new TCanvas("GEANEvsCandINFO");
  c8->Divide(3,2);
  c8->cd(1);
  hSeedGEANEX->Draw();
  c8->cd(2);
  hSeedGEANEY->Draw();
  c8->cd(3);
  hSeedGEANEZ->Draw();
  c8->cd(4);
  hSeedGEANETheta->Draw();
  TLine *uplim = new TLine(0.05, 0, 0.05, 0.1);
  TLine *downlim = new TLine(0.03, 0, 0.03, 0.1);
  uplim->SetLineColor(kRed);
  downlim->SetLineColor(kRed);
  uplim->SetLineWidth(2);
  downlim->SetLineWidth(2);
  uplim->Draw();
  downlim->Draw();
  // ---- Count number events in range [3-8] mrad ------------------------
  Int_t binx1, binx2;
  Int_t binmax = hSeedGEANETheta->GetNbinsX();
  for(int bin=0;bin<binmax;bin++){
    Double_t binCenter = hSeedGEANETheta->GetBinCenter(bin);
    if(binCenter>0.05){
    //    if(binCenter>5){
      binx2 = bin;
      break;
    }
  }
  for(int bin=0;bin<binmax;bin++){
    Double_t binCenter = hSeedGEANETheta->GetBinCenter(bin);
    if(binCenter>0.03){
      binx1 = bin;
      break;
    }
  }
  //  cout<<"binmax = "<<binmax<<" binx1 = "<<binx1<<" binx2 = "<<binx2<<endl;
  Double_t nall = hSeedGEANETheta->Integral(0, binmax);
  Double_t neff = hSeedGEANETheta->Integral(binx1, binx2);
  // cout<<"Number of track-candidate with #theta from [30-50] mrad = "<<neff<<endl;
  cout<<"Total number of track-candidate = "<<nall<<endl;
 
  //----------------------------------------------------------------------
  c8->cd(5);
  hSeedGEANEPhi->Draw();
  c8->cd(6);
  hSeedThetaPhi->Draw();
  c8->Write();
  c8->Close();

  TCanvas *c9 = new TCanvas("DCA");
  c9->Divide(3,2);
  c9->cd(1);
  hResDCA->Draw();
  c9->cd(2);
  hErrDCA->Draw();
  c9->cd(3);
  hPullDCA->Draw();
  c9->Write();
  c9->Close();

  hangMCRec->Write();
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
  hangMCRec->Write();
  hPointXY->Write();
  hPointXYcut->Write();
  hSeedGEANEX->Write();
  hSeedGEANEY->Write();
  hSeedGEANEZ->Write();
  hSeedGEANER->Write();
  hSeedGEANETheta->Write();
  hSeedGEANEPhi->Write();
  hSeedThetaPhi->Write();
  hRecGEANEX->Write();
  hRecGEANEY->Write();
  hRecGEANEZ->Write();
  hchi2Errx->Write();
  hchi2Erry->Write();
  
  TCanvas *c10 = new TCanvas("ThetaPhiDistr");
  c10->Divide(4,2);
  // c10->cd(1);
  //  hSeedThetaDiffIDs->Draw("colz");
  // c10->cd(2);
  //  hSeedPhiDiffIDs->Draw("colz");
  // c10->cd(1);
  // hMCThetaGEANETheta->Draw("colz");
  // c10->cd(2);
  // hMCPhiGEANEPhi->Draw("colz");
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

   TCanvas *c11 = new TCanvas("HitsUsing");
   c11->Divide(2,2);
   c11->cd(1);
   hntrkMCtrkID->Draw("colz");
   c11->cd(2);
   // hntrkMCtrkID->ProjectionY();
   // hntrkMCtrkID_py->Draw();
   c11->cd(3);
   // hntrkMCtrkID->ProjectionX();
   // hntrkMCtrkID_px->Draw();
   c11->Write();
   c11->Close();

  TCanvas *c12 = new TCanvas("TrkLin_and_MC");
  c12->Divide(3,3);
  c12->cd(1);
  hResLumiTrkMom->Fit(funrp,"r");
  hResLumiTrkMom->Draw();
  c12->cd(2);
  hResLumiTrkTheta->Fit(funrth,"r");
  hResLumiTrkTheta->Draw();
  c12->cd(3);
  hResLumiTrkPhi->Fit(funrphi,"r");
  hResLumiTrkPhi->Draw();
  c12->cd(4);
  hResLumiTrkPointX->Fit(funcoord,"r");
  hResLumiTrkPointX->Draw();
  c12->cd(5);
  hResLumiTrkPointY->Fit(funcoord,"r");
  hResLumiTrkPointY->Draw();
  c12->cd(6);
  hResLumiTrkPointZ->Fit(funcoord,"r");
  hResLumiTrkPointZ->Draw();
  c12->cd(7);
  hResLumiTrkPointPx->Fit(funp,"r");
  hResLumiTrkPointPx->Draw();
  c12->cd(8);
  hResLumiTrkPointPy->Fit(funp,"r");
  hResLumiTrkPointPy->Draw();
  c12->cd(9);
  hResLumiTrkPointPz->Fit(funp,"r");
  hResLumiTrkPointPz->Draw();
  c12->Write();
  c12->Close();

 TCanvas *c13 = new TCanvas("TrkLin_and_MC_pulls");
 c13->Divide(3,2);
 c13->cd(1);
 hResLumiTrkPointXPull->Fit(funp,"r");
 hResLumiTrkPointXPull->Draw();
 c13->cd(2);
 hResLumiTrkPointYPull->Fit(funp,"r");
 hResLumiTrkPointYPull->Draw();
 c13->cd(3);
 hResLumiTrkPointZPull->Fit(funp,"r");
 hResLumiTrkPointZPull->Draw();
 c13->cd(4);
 hResLumiTrkPointPxPull->Fit(funp,"r");
 hResLumiTrkPointPxPull->Draw();
 c13->cd(5);
 hResLumiTrkPointPyPull->Fit(funp,"r");
 hResLumiTrkPointPyPull->Draw();
 c13->cd(6);
 hResLumiTrkPointPzPull->Fit(funp,"r");
 hResLumiTrkPointPzPull->Draw();
 c13->Write();
 c13->Close();

 TCanvas *c14 = new TCanvas("TrkIP_andLin_and_MC_pulls");
 c14->Divide(3,2);
 c14->cd(1);
 //Grey = Trk near IP
 //Blue = Trk near Lumi
 hPullPointX->SetLineWidth(2);
 hPullPointX->SetLineColor(12);
 hPullPointX->SetFillColor(12);
 hPullPointX->SetFillStyle(3003);
 hPullPointX->Draw();
 hResLumiTrkPointXPull->SetLineColor(4);
 hResLumiTrkPointXPull->SetLineWidth(2);
 hResLumiTrkPointXPull->Draw("same");
 c14->cd(2);
 hPullPointY->SetLineWidth(2);
 hPullPointY->SetLineColor(12);
 hPullPointY->SetFillColor(12);
 hPullPointY->SetFillStyle(3003);
 hPullPointY->Draw();
 hResLumiTrkPointYPull->SetLineColor(4);
 hResLumiTrkPointYPull->SetLineWidth(2);
 hResLumiTrkPointYPull->Draw("same");
 c14->cd(3);
 hPullPointZ->SetLineWidth(2);
 hPullPointZ->SetLineColor(12);
 hPullPointZ->SetFillColor(12);
 hPullPointZ->SetFillStyle(3003);
 hPullPointZ->Draw();
 hResLumiTrkPointZPull->SetLineColor(4);
 hResLumiTrkPointZPull->SetLineWidth(2);
 hResLumiTrkPointZPull->Draw("same");
 c14->cd(4);
 hPullPointPx->SetLineWidth(2);
 hPullPointPx->SetLineColor(12);
 hPullPointPx->SetFillColor(12);
 hPullPointPx->SetFillStyle(3003);
 hPullPointPx->Draw();
 hResLumiTrkPointPxPull->SetLineColor(4);
 hResLumiTrkPointPxPull->SetLineWidth(2);
 hResLumiTrkPointPxPull->Draw("same");
 c14->cd(5);
 hPullPointPy->SetLineWidth(2);
 hPullPointPy->SetLineColor(12);
 hPullPointPy->SetFillColor(12);
 hPullPointPy->SetFillStyle(3003);
 hPullPointPy->Draw();
 hResLumiTrkPointPyPull->SetLineColor(4);
 hResLumiTrkPointPyPull->SetLineWidth(2);
 hResLumiTrkPointPyPull->Draw("same");
 c14->cd(6);
 hPullPointPz->SetLineWidth(2);
 hPullPointPz->SetLineColor(12);
 hPullPointPz->SetFillColor(12);
 hPullPointPz->SetFillStyle(3003);
 hPullPointPz->Draw();
 hResLumiTrkPointPzPull->SetLineColor(4);
 hResLumiTrkPointPzPull->SetLineWidth(2);
 hResLumiTrkPointPzPull->Draw("same");
 c14->Write();
 c14->Close();


  hSeedThetaDiffIDs->Write();
  hSeedPhiDiffIDs->Write();
  hMCThetaGEANETheta->Write();
  hMCPhiGEANEPhi->Write();
  hMCThetaResTheta->Write();
  hMCPhiResPhi->Write();
  
  hDiffIDsPointX->Write();
  hDiffIDsPointY->Write();
  hDiffIDsPointZ->Write();
  
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

  hMCtrkvshits->Write();
  hntrkMCtrkID->Write();
  hchi2nTrkCand->Write();
  hgoodPhichi2->Write();
  hgoodThetachi2->Write();
  hchi2MCdiffID->Write();
  hnhitsvsIDs->Write();
  hResLumiTrkMom->Write();
  hResLumiTrkTheta->Write();
  hResLumiTrkPhi->Write();
  hResLumiTrkThetaPull->Write();
  hResLumiTrkPhiPull->Write();
  hResLumiTrkPointX->Write();
  hResLumiTrkPointY->Write();
  hResLumiTrkPointZ->Write();
  hResLumiTrkPointPx->Write();
  hResLumiTrkPointPy->Write();
  hResLumiTrkPointPz->Write();
  hResLumiTrkPointXPull->Write();
  hResLumiTrkPointYPull->Write();
  hResLumiTrkPointZPull->Write();
  hResLumiTrkPointPxPull->Write();
  hResLumiTrkPointPyPull->Write();
  hResLumiTrkPointPzPull->Write();
  //  hMomMC->Write();
  hResZResPhi->Write();
  nrecpointall->Write();
  nrecdirall->Write();

  TCanvas *c134 = new TCanvas("TrkLinparams");
 c134->Divide(2,2);
 c134->cd(1);
 hLumiTrkA->Draw();
 c134->cd(2);
 hLumiTrkB->Draw();
 c134->cd(3);
 hLumiTrkC->Draw();
 c134->cd(4);
 hLumiTrkD->Draw();
 c134->Write();
 c134->Close();
 hLumiTrkA->Write(); 
 hLumiTrkB->Write(); 
 hLumiTrkC->Write(); 
 hLumiTrkD->Write(); 
 hMCThetaResSeedTheta->Write();
 hMCPhiResSeedPhi->Write();
 hntrkmissedPhiTheta->Write();
 hntrkmissed_I->Write();
 hntrkghost_I->Write();
 hntrkmissed_II->Write();
 hntrkghost_II->Write();
 hntrkgood_I->Write();
 hntrkgood_II->Write();
 heffPhiTheta->Write();
 nmomMC->Write();
 ntuprecTrk->Write();
 ntupMCTrk->Write();
 hnhits->Write();
 nsectors->Write();
 hPullPointXphi->Write();
 hPullPointYphi->Write();
 hPullPointXtheta->Write();
 hPullPointYtheta->Write();
 f->Close();
 cout<<"Number of events with low number of hits (less then 3 per trk): "<<glBadEv<<endl;
 cout<<"Number of trks where GEANE failed: "<<glBADGEANE<<endl;
 cout<<"Total number of noise hits = "<<glNoisehit<<endl;
}
