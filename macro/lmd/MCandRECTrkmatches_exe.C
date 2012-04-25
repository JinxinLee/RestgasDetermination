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
void combitransFromLumiFrame(TVector3& hitPos){
  //do the transformation from lab frame to LUMI frame (with z-axis perp. to lumi planes)
  const Double_t  kHalfFoilThickness  = 0.0075; // Thickness of sensitive foil (cm)
  const Double_t  kTransZ = 1100.; //(cm) //move at z-position
  const Double_t  kRotUmZ = 476.03; //(cm) //z-point to rotate
  const Double_t  kTransX = 25; //(cm) //move at x-position
  const Double_t  kRot =  0.040596358401388; // 2.326 degree  = 4.05963584013881024e-02 rad
  TVector3 LumiTrans(0,0,kTransZ-kRotUmZ);
  hitPos +=LumiTrans;
  hitPos.RotateY(kRot);
  LumiTrans = TVector3(0,0,kRotUmZ);
  hitPos +=LumiTrans;
  // cout<<"!!! NEW HIT position in LUMI frame!!! "<<endl;
  //  hitPos.Print();
}
TMatrixD rotateFromLumiFrame(TMatrixD& hitCov){
  Double_t theta=-2.326;
  Double_t degrad = TMath::Pi()/180.;
  Double_t sintheta = TMath::Sin(degrad*theta);
  Double_t costheta = TMath::Cos(degrad*theta);
  TMatrixD rot(3,3);// Rotation around Y axis
  rot[0][0]= costheta;
  rot[0][1]= 0;
  rot[0][2]= sintheta;
  rot[1][0]= 0;
  rot[1][1]= 1;
  rot[1][2]= 0;
  rot[2][0]= -sintheta;
  rot[2][1]= 0;
  rot[2][2]= costheta;
  TMatrixD result = rot;
  result.T();
  result*=hitCov;
  hitCov = result;
  result*=rot;

  // cout<<"hitCov:"<<endl;
  // hitCov.Print();
  // cout<<"result:"<<endl;
  // result.Print();
  return result;
}


void rotateFromLumiFrame(TVector3& hitPos, bool err){
  TMatrixD hitMtx(3,3);
  if(err){
    hitMtx[0][0] = hitPos[0]*hitPos[0];
    hitMtx[1][1] = hitPos[1]*hitPos[1];
    hitMtx[2][2] = hitPos[2]*hitPos[2];
    TMatrixD res = rotateFromLumiFrame(hitMtx);
    hitPos = TVector3(sqrt(res(0,0)),sqrt(res(1,1)),sqrt(res(2,2)));
  }
  else{
    hitMtx[0][0] = hitPos[0];
    hitMtx[1][0] = hitPos[1];
    hitMtx[2][0] = hitPos[2];
    TMatrixD res = rotateFromLumiFrame(hitMtx);
    hitPos = TVector3(hitMtx(0,0),hitMtx(1,0),hitMtx(2,0));
  }
}

using namespace std;
int main(int __argc,char *__argv[]) {
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
    
    std::cout << "This is script for comparision reconstructed and simulated tracks with parameters\n"
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
  // TClonesArray* geaneArray =new TClonesArray("FairTrackParP");
  TClonesArray* geaneArray =new TClonesArray("FairTrackParH");
  tgeane.SetBranchAddress("GeaneTrackFinal",&geaneArray);  //Tracks with parabolic parametrisation
  
  //--- Output histogram -----------------------------------------------------
  TH1 *hchi2 = new TH1F("hchi2","#chi^2 for reconstructed tracks;#chi^2;",5e2,0,50);
  TH2 *hnRecnMC = new TH2F("hnRecnMC","Number reconstracted tracks vs. Number simulated tracks; N_{MC}; N_{rec}",
			   100,0,100,100,0,100);
  TH1 *hDiffIDs = new TH1F("hDiffIDs","Number of track-candidates with hits from diff. MC-track;N_{IDs}",10,0,10);
  TH2 *hMCtrkvshits = new TH2F("hMCtrkvshits","Number of simulated tracks vs. Number of rec. hits",
			       1000,0,1000,100,0,100);
  TH1 *hntrkcand = new TH1F("hntrkcand","Number of track-candidates per event;N_{trk-cand}",100,0,100);
  TH1 *hntrk = new TH1F("hntrk","Number of tracks per event;N_{trk}",100,0,100);
  TH2 *hntrkcandvsIDs = new TH2F("hntrkcandvsIDs","Number of track-candidates per event vs Number of track-candidates with hits from diff. MC-track;N_{IDs};N_{trk-cand}",10,0,10,100,0,100);
  TH2 *hntrkcandvsMC = new TH2F("hntrkcandvsMC","Number of per event vs. Number of simulated tracks; N_{MC};N_{trk-cand}",100,0,100,100,0,100);

  TH2 *hnhitsvsIDs = new TH2F("hnhitsvsIDs","Number of hits per track-candidate vs Number of track-candidates with hits from diff. MC-track;N_{IDs};Number of hits",10,0,10,4,2,6);
  
  TH1 * hallPhi = new TH1F("hallPhi","#phi for all MC-track;#phi, rad",20,-355./113,355./113);
  TH1 * hgoodPhi = new TH1F("hgoodPhi","#phi for good track-candidates",20,-355./113,355./113);
  
  TH1 * hallTheta = new TH1F("hallTheta","#theta for all MC-track;#theta, rad",20,3.5e-3,8.5e-3);
  TH1 * hgoodTheta = new TH1F("hgoodTheta","#theta for good track-candidates",20,3.5e-3,8.5e-3);

  TH2 * hgoodPhichi2 = new TH2F("hgoodPhichi2","#phi for good track-candidates vs. #chi^{2} ",5e2,0,50,20,-355./113,355./113);
  TH2 * hgoodThetachi2 = new TH2F("hgoodThetachi2","#theta for good track-candidates vs. #chi^{2} ",5e2,0,50,20,3.5e-3,8.5e-3);
  
  TH1 *hMomMC = new TH1F("hMomMC","P_{MC};P,GeV/c",1e3,0,16.);

  TH1 *hResMom = new TH1F("hResMom","P_{MC}-P_{rec};#deltaP,GeV/c",1e3,-1e-4,1e-4);
  TH1 *hErrMom = new TH1F("hErrMom","#sigma_{P};#sigmaP,GeV/c",1e3,0,1e-3);
  TH1 *hPullMom = new TH1F("hPullMom","(P_{MC}-P_{rec})/#sigma_{P};",1e3,-1e1,1e1);
  TH1 *hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e3,-1e-3,1e-3);
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
  TH1 *hResPointZ = new TH1F("hResPointZ","Z_{MC}-Z_{rec};#deltaZ,cm",1e2,-0.015,0.015);

  TH1 *hResLumiTrkPointX = new TH1F("hResLumiTrkPointX","X_{MC}-X_{rec}(near Lumi);#deltaX,cm",1e3,-0.02,0.02);
  TH1 *hResLumiTrkPointY = new TH1F("hResLumiTrkPointY","Y_{MC}-Y_{rec}(near Lumi);#deltaY,cm",1e3,-0.02,0.02);
  TH1 *hResLumiTrkPointZ = new TH1F("hResLumiTrkPointZ","Z_{MC}-Z_{rec}(near Lumi);#deltaZ,cm",1e3,-0.02,0.02);

  TH1 *hResLumiTrkPointPx = new TH1F("hResLumiTrkPointPx","Px_{MC}-Px_{rec}(near Lumi);#deltaPx, GeV/c",1e2,-0.002,0.002);
  TH1 *hResLumiTrkPointPy = new TH1F("hResLumiTrkPointPy","Py_{MC}-Py_{rec}(near Lumi);#deltaPy, GeV/c",1e2,-0.002,0.002);
  TH1 *hResLumiTrkPointPz = new TH1F("hResLumiTrkPointPz","Pz_{MC}-Pz_{rec}(near Lumi);#deltaPz, GeV/c",1e2,-0.0001,0.0001);

  TH1 *hResLumiTrkPointXPull = new TH1F("hResLumiTrkPointXPull","X_{MC}-X_{rec}(near Lumi);#deltaX,cm",1e2,-10.,10.);
  TH1 *hResLumiTrkPointYPull = new TH1F("hResLumiTrkPointYPull","Y_{MC}-Y_{rec}(near Lumi);#deltaY,cm",1e2,-10.,10.);
  TH1 *hResLumiTrkPointZPull = new TH1F("hResLumiTrkPointZPull","Z_{MC}-Z_{rec}(near Lumi);#deltaZ,cm",1e2,-10.,10.);

  TH1 *hResLumiTrkPointPxPull = new TH1F("hResLumiTrkPointPxPull","Px_{MC}-Px_{rec}(near Lumi);#deltaPx, GeV/c",1e2,-10,10.);
  TH1 *hResLumiTrkPointPyPull = new TH1F("hResLumiTrkPointPyPull","Py_{MC}-Py_{rec}(near Lumi);#deltaPy, GeV/c",1e2,-10,10);
  TH1 *hResLumiTrkPointPzPull = new TH1F("hResLumiTrkPointPzPullS","Pz_{MC}-Pz_{rec}(near Lumi);#deltaPz, GeV/c",1e2,-10,10);

  TH1 *hErrPointX = new TH1F("hErrPointX","#sigma_{X};#sigmaX,cm",1e3,0,1.);
  TH1 *hErrPointY = new TH1F("hErrPointY","#sigma_{Y};#sigmaY,cm",1e3,0,1.);
  TH1 *hErrLinPointX = new TH1F("hErrLinPointX","#sigma_{X};#sigmaX,cm",1e3,0,0.01);
  TH1 *hErrLinPointY = new TH1F("hErrLinPointY","#sigma_{Y};#sigmaY,cm",1e3,0,0.001);
  TH1 *hErrPointZ = new TH1F("hErrPointZ","#sigma_{Z};#sigmaZ,cm",1e2,0,0.01);
  TH1 *hPullPointX = new TH1F("hPullPointX","(X_{MC}-X_{rec})/#sigma_{X};",1e2,-10,10);
  TH1 *hPullPointY = new TH1F("hPullPointY","(Y_{MC}-Y_{rec})/#sigma_{Y};",1e2,-10,10);
  TH1 *hPullPointZ = new TH1F("hPullPointZ","(Z_{MC}-Z_{rec})/#sigma_{Z};",1e2,-10,10);
  
  TH1 *hResPointPx = new TH1F("hResPointPx","Px_{MC}-Px_{rec};#deltaPx, GeV/c",1e2,-0.01,0.01);
  TH1 *hErrPointPx = new TH1F("hErrPointPx","#sigma_{Px};#sigmaPx, GeV/c",1e3,0,0.01);
  TH1 *hPullPointPx = new TH1F("hPullPointPx","(Px_{MC}-Px_{rec})/#sigma_{Px};",1e2,-10,10);
  
  TH1 *hResPointPy = new TH1F("hResPointPy","Py_{MC}-Py_{rec};#deltaPy, GeV/c",1e2,-0.01,0.01);
  TH1 *hErrPointPy = new TH1F("hErrPointPy","#sigma_{Py};#sigmaPy, GeV/c",1e3,0,0.01);
  TH1 *hPullPointPy = new TH1F("hPullPointPy","(Py_{MC}-Py_{rec})/#sigma_{Py};",1e2,-10,10);
  
  TH1 *hResPointPz = new TH1F("hResPointPz","Pz_{MC}-Pz_{rec};#deltaPz, GeV/c",1e2,-1e-3,1e-3);
  TH1 *hErrPointPz = new TH1F("hErrPointPz","#sigma_{Pz};#sigmaPz, GeV/c",1e3,0,1e-3);
  TH1 *hPullPointPz = new TH1F("hPullPointPz","(Pz_{MC}-Pz_{rec})/#sigma_{Pz};",1e2,-10,10);
  
  TH1 *hResDCA = new TH1F("hDCA","|DCA|;|DCA|,cm",1e3,0,10.);
  TH1 *hErrDCA = new TH1F("hErrDCA","#sigma_{DCA};#sigmaDCA,cm",1e3,0,1.);
  TH1 *hPullDCA = new TH1F("hPullDCA","|DCA|/#sigma_{DCA};",1e2,0,10);
  
  TH2 *hPointXY = new TH2F("hPointXY","PCA;X,cm;Y,cm",1e3,-10,10.,1e3,-10,10.);
  TH2 *hPointXYcut = new TH2F("hPointXYcut","PCA;#X,cm;#Y,cm",1e3,-1.,1.,1e3,-1.,1.);
  TH2 *hErrPointXY = new TH2F("hErrPointXY","#sigma_{PCA};#sigmaX,cm;#sigmaY,cm",1e3,0,30.,1e3,0,30.);
  
  TH2 *hHitIDHitX = new TH2F("hHitIDHitX",";MC_{ID} ;hit_{X}, cm;",6,-1,5,1e3,24.,45.);
  TH2 *hHitIDHitY = new TH2F("hHitIDHitY",";MC_{ID} ;hit_{Y}, cm;",6,-1,5,1e3,-10.,10.);
  
  TH2 *hRecGEANEX = new TH2F("hRecGEANEX",";X_{rec}, cm;X_{GEANE}, cm",1e3,-15.,15.,1e3,-500,500.);
  TH2 *hRecGEANEY = new TH2F("hRecGEANEY",";Y_{rec}, cm;Y_{GEANE}, cm",1e3,-15.,15.,1e3,-500,500.);
  TH2 *hRecGEANER = new TH2F("hRecGEANER",";R_{rec}, cm;R_{GEANE}, cm",1e3,0,10.,1e3,0,10.);
  TH2 *hRecGEANETheta = new TH2F("hRecGEANETheta",";#theta_{rec}, rad;#theta_{GEANE}, rad",1e3,0,1,1e3,0,1);
  TH2 *hRecGEANEPhi = new TH2F("hRecGEANEPhi",";#phi_{rec}, rad;#phi_{GEANE}, rad",1e3,-7,7,1e3,-7,7);
  TH2 *hRecThetaPhi = new TH2F("hRecThetaPhi",";#theta_{rec}, rad;#phi_{rec}, rad",1e3,0,1.,1e3,-7,7);
  
  TH2 *hSeedGEANEX = new TH2F("hSeedGEANEX",";X_{cand}, cm;X_{GEANE}, cm",1e3,-15.,15.,1e3,-5.,5.);
  TH2 *hSeedGEANEY = new TH2F("hSeedGEANEY",";Y_{cand}, cm;Y_{GEANE}, cm",1e3,-15,15.,1e3,-5.,5.);
  TH2 *hSeedGEANER = new TH2F("hSeedGEANER",";R_{cand}, cm;R_{GEANE}, cm",1e3,0,10.,1e3,0,10.);
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
  
  TH2 *hMCThetaResTheta = new TH2F("hMCThetaResTheta",";#theta_{MC}, rad;#Delta#theta, rad",1e2,0,0.012,1e2,-1e-3,1e-3);
  TH2 *hMCPhiResPhi = new TH2F("hMCPhiResPhi",";#phi_{MC}, rad;#Delta#phi, rad",1e2,-3.15,3.15,1e2,-1e-1,1e-1);
  TH2 *hResZResPhi = new TH2F("hResZResPhi",";#Delta#phi, rad;#Delta Z, cm",1e3,-2e-1,2e-1,1e3,-0.02,0.02);
  TH2 *hntrkMCtrkID = new TH2F("hntrkMCtrkID","Number of participation this MCid in rec.tracks;ID;Number of rec.trks",10,0,10,10,0,10);
  TH2 *hchi2MCdiffID = new TH2F("hchi2MCdiffID","number diff MCid in rec.tracks vs. #chi^{2};#chi^{2};N",5e2,0,50,10,0,10);

  //-----------------------------------------------------------------------------
  
  TH2 *hchi2Errx = new TH2F("hchi2Errx","#sigma_{X} vs. #chi^2 for reconstructed tracks;#chi^2;#sigma_{X}, cm",
			    1e3,0,100,2e1,0,0.2);
  TH2 *hchi2Erry = new TH2F("hchi2Erry","#sigma_{Y} vs. #chi^2 for reconstructed tracks;#chi^2;#sigma_{X}, cm",
			    1e3,0,100,2e1,0,0.2);
 TH2 *hchi2nTrkCand = new TH2F("hchi2nTrkCand"," ;Number of trk-cand;#chi^2",
			    30,0,30,5e2,0,50.);
  
  for (Int_t j=0; j<nEvents; j++){
    //  cout<<"Event #"<<j<<endl;
    // Read GEANE & MC info -----------------------------------------------------------------
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
    tTrkCand.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);
    
    const int nGeaneTrks = geaneArray->GetEntriesFast();
    const int nParticles = true_tracks->GetEntriesFast();
    const int numTrk = nGeaneTrks;
    const int nRecHits = rechit_array->GetEntriesFast();
    const int nTrkCandidates = trkcand_array->GetEntriesFast();
    const int nRecTrks = rec_trk->GetEntriesFast();
    if(verboseLevel>0)  cout<<"Event #"<<j<<" has "<<nParticles<<" true particles, "<<nTrkCandidates<<" trk-cands, "<<numTrk<<" tracks and "
			    <<nGeaneTrks<<" geane Trks!"<<endl;
    double chi2Cont[5*numTrk];
    double ndiffIDCont[5*numTrk];
    //  if(nParticles!=numTrk) continue;
   
    hMCtrkvshits->Fill(nRecHits,nParticles);
    if(nTrkCandidates>numTrk) cout<<"Event #"<<j<<" has "<<nTrkCandidates<<" trk-cands and "<<numTrk<<" tracks!"<<endl;
    hnRecnMC->Fill(nParticles,nGeaneTrks);
    hntrkcand->Fill(nTrkCandidates);
    hntrkcandvsMC->Fill(nParticles,nTrkCandidates);
    
    /// Set MC ID for each track ----------------------------------------------------
    Int_t nRecGEANEtrk = 0;
    int MCtrk[numTrk];
    for(int nk=0;nk<numTrk;nk++)
      MCtrk[nk]=0;
    for (Int_t iN=0; iN<nGeaneTrks; iN++){
      vector<int> MCtrkID;
      Int_t diffIDs=1;
      FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(iN);
      Double_t lyambda = fRes->GetLambda();
      if(lyambda==0){
	cout<<"GEANE didn't propagate this trk!"<<endl;
	cout<<"Event #"<<j<<" diffIDs = "<<diffIDs<<endl;}
      if(lyambda==0) continue;
      PndLinTrack *trk = (PndLinTrack*)rec_trk->At(iN);
      Double_t errparlin[6];
      trk->GetParErr(errparlin);
      Int_t candID = trk->GetTCandID();
      double chi2 = trk->GetChiSquare();
      chi2Cont[iN] = chi2;
      hchi2nTrkCand->Fill(nTrkCandidates,chi2);
      //  if(chi2>5) continue; // !!! TEST
      PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);
      const int Ntrkcandhits= trkcand->GetNHits();
      PndSdsMCPoint* MCPointHit;
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
	if(iHit==0) MCPointHit = MCPoint;
	double zTop = MCPoint->GetZ();
	double xTop = MCPoint->GetX();
	double yTop = MCPoint->GetY();
	///Bottom cluster
	Int_t  botIndex = myHit->GetBotIndex();
	PndSdsClusterStrip* myClusterBot =  (PndSdsClusterStrip*)(fStripClusterArray->At(botIndex));
	PndSdsDigiStrip* astripdigiBot = (PndSdsDigiStrip*)(fStripDigiArray->At(myClusterBot->GetDigiIndex(0)));
	if (astripdigiBot->GetIndex(0) == -1) continue; // sort out noise
	PndSdsMCPoint* MCPointBot = (PndSdsMCPoint*)(true_points->At(astripdigiBot->GetIndex(0)));
	double zBot = MCPointBot->GetZ();
	double xBot = MCPointBot->GetX();
	double yBot = MCPointBot->GetY();
	// cout<<"zTop - zBot = "<<zTop-zBot<<endl;
	// cout<<"xTop - xBot = "<<xTop-xBot<<endl;
	// cout<<"yTop - yBot = "<<yTop-yBot<<endl;
	int MCidBOT = MCPointBot->GetTrackID();
	//	if(MCidTOP!=MCidBOT) continue;
	MCtrkID.push_back(MCidTOP);
	MCtrkID.push_back(MCidBOT);
      }
      
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
      // if(ch){
      // 	cout<<"Number ev = "<<i<<endl;
       // cout<<"MC IDs: ";
       // for(vector<int>::iterator it=MCtrkID.begin(); it<MCtrkID.end(); it++)
       // 	 cout<<" "<< *it;
       // cout<<" !"<<endl;
      // }
      // cout<<"Number of element changes in this event is "<<nch<<endl;
      ///--------------------------------------------------------------------------
      
      /// Counting number of diff MC ids ----------------------------------------      
      Int_t prevID = MCtrkID[0];
      for(int nk=0;nk<numTrk;nk++){
	if(MCtrkID[0]==nk)
	  MCtrk[nk]=MCtrk[nk]+1;
      }
      diffIDs = 1;
      for(Int_t n=1; n<MCtrkID.size(); n++){ 
	if(prevID<MCtrkID[n]){
	  diffIDs++;
	  prevID=MCtrkID[n];
	  for(int nk=0;nk<numTrk;nk++){
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
      
      /// Comporision MC, trk-candidates and REConstructed tracks --------------------------------------------
      TVector3 posSeed = trkcand->getPosSeed();
      TVector3 dirSeed = trkcand->getDirSeed();
      hSeedThetaDiffIDs->Fill(diffIDs,dirSeed.Theta());
      hSeedPhiDiffIDs->Fill(diffIDs,dirSeed.Phi());
      //  cout<<"diffIDs="<<diffIDs<<endl;
      // if(diffIDs<2){ // Hits with only one type of MC IDs are combined in track-candidate
      //if(diffIDs<3){ // Hits with only two type of MC IDs are combined in track-candidate
      //if(diffIDs>1){ // Hits with diff. types of MC IDs are combined in track-candidate
      if(diffIDs>-1){ // All tracks
	//	if(diffIDs>3) cout<<"Event #"<<j<<" diffIDs = "<<diffIDs<<endl;
	/// Try to find connection between position of -----------
	/// reconstructed track and track after GEANE propagation
	TVector3 pos_rec_trk = trk->GetStartVec();
	TVector3 dir_rec_trk = trk->GetDirectionVec();
	TVector3 pos_prop_geane_trk = fRes->GetPosition();
	//	if(fabs(pos_prop_geane_trk.X())<0.5 && fabs(pos_prop_geane_trk.Y())<0.5 && fabs(pos_prop_geane_trk.Z())<0.05){
	Double_t theta_prop_geane_trk = TMath::Pi()/2. - lyambda;
	Double_t phi_prop_geane_trk = fRes->GetPhi();
	hRecGEANEX->Fill(pos_rec_trk.X(),pos_prop_geane_trk.X());
	hRecGEANEY->Fill(pos_rec_trk.Y(),pos_prop_geane_trk.Y());
	hRecGEANER->Fill(pos_rec_trk.Perp(),pos_prop_geane_trk.Perp());
	hRecGEANETheta->Fill(dir_rec_trk.Theta(), theta_prop_geane_trk);
	hRecGEANEPhi->Fill(dir_rec_trk.Phi(), phi_prop_geane_trk);
	hRecThetaPhi->Fill(dir_rec_trk.Theta(),dir_rec_trk.Phi());
	
	hSeedGEANEX->Fill(posSeed.X(),pos_prop_geane_trk.X());
	hSeedGEANEY->Fill(posSeed.Y(),pos_prop_geane_trk.Y());
	hSeedGEANER->Fill(posSeed.Perp(),pos_prop_geane_trk.Perp());
	hSeedGEANETheta->Fill(dirSeed.Theta(), theta_prop_geane_trk);
	hSeedGEANEPhi->Fill(dirSeed.Phi(), phi_prop_geane_trk);
	hSeedThetaPhi->Fill(dirSeed.Theta(),dir_rec_trk.Phi());
	
	///-------------------------------------------------------
	
	/// CUT: Check position and coordinates errors of PCA ---------------------------------------
	TVector3 PosRec = fRes->GetPosition();
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
	int MCidforREC = MCtrkID[0];
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(MCidforREC);
	Int_t mcID = mctrk->GetPdgCode();
	//	  if(mcID!=-2212) continue;
	TVector3 MomMC = mctrk->GetMomentum();
	// cout<<" "<<endl;
	// cout<<" ------------------ "<<endl;
	// cout<<"MC trk dir:"<<endl;
	// MomMC.Print();
	//	if(MomMC)
	Double_t thetaMC = MomMC.Theta();
	Double_t phiMC = MomMC.Phi();
	// hgoodTheta->Fill(thetaMC);
	// hgoodPhi->Fill(phiMC);
	///------------------------------------------------------------------------------------
	
	/// Read track-parameters after back-propagation ---------------------------------------
	Double_t thetaBP = TMath::Pi()/2. - lyambda;
	Double_t err_lyambda = fRes->GetDLambda();
	// Double_t err_lyambda = errMom.Theta();
	Double_t phiBP = fRes->GetPhi();
	Double_t err_phi = fRes->GetDPhi();
	// Double_t err_phi = errMom.Phi();
	Double_t errMomRecBP = fRes->GetDQp();
	// Double_t errMomRecBP =  errMom.Mag();
	TVector3 MomRecBP = fRes->GetMomentum();
	//	if(fabs(MomRecBP.Mag()-15)<2e-5) continue; //TEST!
	// Double_t thetaBP = MomRecBP.Theta();
	// Double_t phiBP = MomRecBP.Phi();
	Double_t errPx = fRes->GetDPx();
	Double_t errPy = fRes->GetDPy();
	Double_t errPz = fRes->GetDPz();
	
	Double_t resTheta = thetaBP-thetaMC;
	///=== Looking for some tracks ===
	//	if(resTheta>0.02) continue; 
	//	if((dirSeed.Theta()<5e-5 && fabs(dirSeed.Phi())<0.26) && (fabs(PosRec.X())<0.4 && fabs(PosRec.Y())<0.4)){
	//	if((dirSeed.Theta()<5e-2 && fabs(dirSeed.Phi())<0.26)){
	//	  cout<<"dirSeed.Theta() = "<<dirSeed.Theta()<<" dirSeed.Phi() = "<<dirSeed.Phi()<<endl;
	//  cout<<"PosRec.X() = "<<PosRec.X()<<" PosRec.Y() = "<<PosRec.Y()<<" PosRec.Z() = "<<PosRec.Z()<<endl;
	// if(fabs(PosRec.X())>0.5 || fabs(PosRec.Y())>0.5 || fabs(PosRec.Z())>0.06) continue;
	//	if((dirSeed.Theta()>3e-2 && dirSeed.Theta()<5e-2) && (fabs(dirSeed.Phi())<0.26)){
	   hgoodTheta->Fill(thetaMC);
	   hgoodPhi->Fill(phiMC);
	   hgoodThetachi2->Fill(chi2,thetaMC);
	   hgoodPhichi2->Fill(chi2,phiMC);

	// hgoodTheta->Fill(thetaBP);
	// hgoodPhi->Fill(phiBP);
	  hMCThetaGEANETheta->Fill(thetaMC,thetaBP);
	  hMCPhiGEANEPhi->Fill(phiMC,phiBP);
	  hMCThetaResTheta->Fill(thetaMC,thetaMC-thetaBP);
	  hMCPhiResPhi->Fill(phiMC,phiMC-phiBP);
	  if((phiMC-phiBP)>0.04) 
	    cout<<"[BAD] Event #"<<j<<" has "<<nTrkCandidates<<" trk-cands and "<<numTrk<<" tracks!"<<endl;
	  hResPointX->Fill(PosRec.X());
	  hResPointY->Fill(PosRec.Y());
	  hResDCA->Fill(PosRec.Mag());
	  Double_t errPCA = TMath::Sqrt(errX*errX+errY*errY+errZ*errZ);
	  hErrDCA->Fill(errPCA);
	  hPullDCA->Fill(PosRec.Mag()/errPCA);
	  hPointXY->Fill(PosRec.X(), PosRec.Y());
	  hPointXYcut->Fill(PosRec.X(), PosRec.Y());
	  hResPointZ->Fill(PosRec.Z());
	  hErrPointX->Fill(errX);
	  hErrPointY->Fill(errY);
	  hErrLinPointX->Fill(errXlin);
	  hErrLinPointY->Fill(errYlin);
	  hErrPointXY->Fill(errX,errY);
	  hErrPointZ->Fill(errZ);
	  hPullPointX->Fill(PosRec.X()/errX);
	  hPullPointY->Fill(PosRec.Y()/errY);
	  hPullPointZ->Fill(PosRec.Z()/errZ);
	  
	  hResPointPx->Fill(MomMC.X()-MomRecBP.X());
	  hResPointPy->Fill(MomMC.Y()-MomRecBP.Y());
	  hResPointPz->Fill(MomMC.Z()-MomRecBP.Z());
	  hErrPointPx->Fill(errPx);
	  hPullPointPx->Fill((MomMC.X()-MomRecBP.X())/errPx);
	  hErrPointPy->Fill(errPy);
	  hPullPointPy->Fill((MomMC.Y()-MomRecBP.Y())/errPy);
	  hErrPointPz->Fill(errPz);
	  hPullPointPz->Fill((MomMC.Z()-MomRecBP.Z())/errPz);
	  ///==================================
	  
	  Double_t resPhi = phiBP-phiMC;
	  Double_t resMom = MomRecBP.Mag()-MomMC.Mag();
	  hResMom->Fill(resMom);
	  hErrMom->Fill(errMomRecBP);
	  hPullMom->Fill(resMom/errMomRecBP);
	  hResTheta->Fill(resTheta);
	  hErrTheta->Fill(err_lyambda);
	  hPullTheta->Fill(resTheta/err_lyambda);
	  hResPhi->Fill(resPhi);
	  hErrPhi->Fill(err_phi);
	  hPullPhi->Fill(resPhi/err_phi);
	  hResZResPhi->Fill(resPhi,PosRec.Z());
	  
	  Double_t angMCRec = MomMC.Angle(MomRecBP); // Angle between two vectors
	  hangMCRec->Fill(angMCRec);
	  hchi2->Fill(chi2);
	  //	}

	 ///Compare lin trk and MC trk in LUMI frame========================
	  // cout<<" "<<endl;
	  // cout<<"=============================="<<endl;
	  TVector3 vtxLumi = trk->GetStartVec();
	  TVector3 vtxLumiErr = trk->GetStartErrVec();
	  TVector3 dirLumi = trk->GetDirectionVec();
	  TVector3 dirLumiErr = trk->GetDirectionErrVec();
	
	  // double dz = 0.001;
	  // double dx = dirLumi.X()*dz;
	  // double dy = dirLumi.Y()*dz;
	  // vtxLumi += TVector3(dx,dy,dz);
	  // cout<<"vtxLumi(before):"<<endl;
	  // vtxLumi.Print();
	  // cout<<"dirLumiErr(before):"<<endl;
	  // dirLumiErr.Print();

	  // //do the transformation from LUMI frame (with z-axis perp. to lumi planes) to lab frame
	  combitransFromLumiFrame(vtxLumi);
	  rotateFromLumiFrame(dirLumi, false);
	  // double dirErrnorm = dirLumiErr.Mag();
	  // dirLumiErr *= 1./dirErrnorm;
	  rotateFromLumiFrame(dirLumiErr, true);
	  //	  dirLumiErr *= dirErrnorm;

	  // double vtxErrnorm = vtxLumiErr.Mag();
	  // vtxLumiErr *= 1./vtxErrnorm;
	  rotateFromLumiFrame(vtxLumiErr, true);
	  // vtxLumiErr *= vtxErrnorm;

	  // cout<<"dirLumi(after):"<<endl;
	  // dirLumi.Print();

	  // cout<<"dirLumiErr(after):"<<endl;
	  // dirLumiErr.Print();
	 
	  double xTrue = MCPointHit->GetX();
	  double yTrue = MCPointHit->GetY();
	  double zTrue = MCPointHit->GetZ();
	  TVector3 vtxLumiMC = TVector3(xTrue,yTrue,zTrue);
	
	
	  double pxTrue =  MCPointHit->GetPx();
	  double pyTrue =  MCPointHit->GetPy();
	  double pzTrue =  MCPointHit->GetPz();
	  TVector3 dirLumiMC = TVector3(pxTrue,pyTrue,pzTrue);
	  hMomMC->Fill(dirLumiMC.Mag());
	  // cout<<"MC hit dir:"<<endl;
	  // dirLumiMC.Print();

	  dirLumiMC *=1./dirLumiMC.Mag();
	  cout<<"vtxLumiMC = "<<endl;
	  vtxLumiMC.Print();


	  // //	  double dz = -0.001;
	  double dz = -0.0017;
	  double dx = dirLumiMC.X()*dz;
	  double dy = dirLumiMC.Y()*dz;
	  vtxLumiMC += TVector3(dx,dy,dz);
	 
	  hResLumiTrkMom->Fill(dirLumiMC.Mag()-dirLumi.Mag());
	  hResLumiTrkTheta->Fill(dirLumiMC.Theta()-dirLumi.Theta());
	  hResLumiTrkPhi->Fill(dirLumiMC.Phi()-dirLumi.Phi());
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
      ///================================================================
      }
      ///-------------------------------------------------------------------------------------
      MCtrkID.clear();
    }
    for(int nk=0;nk<numTrk;nk++){
      hntrkMCtrkID->Fill(nk,MCtrk[nk]);
      hchi2MCdiffID->Fill(chi2Cont[nk],ndiffIDCont[nk]);
      //      if(MCtrk[nk]>1) cout<<"Attention MCtrk["<<nk<<"]="<<MCtrk[nk]<<endl;
    }
 
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

  TCanvas *c2 = new TCanvas("EffINFO");
  c2->Divide(2,2);
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
  c2->cd(2);
  heffTheta->SetMinimum(0.9);
  heffTheta->Draw();
  c2->cd(3);
  hallPhi->SetLineWidth(2);
  hallPhi->SetFillStyle(3354);
  hallPhi->SetFillColor(kBlack);
  hallPhi->Draw();
  hgoodPhi->SetLineWidth(2);
  hgoodPhi->SetLineColor(2);
  hgoodPhi->SetFillColor(kRed);
  hgoodPhi->SetFillStyle(3690);
  hgoodPhi->Draw("same");
  c2->cd(4);
  heffPhi->SetMinimum(0.9);
  heffPhi->Draw();
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

  
  TF1 *funcoord = new TF1("fitcoord","gaus",-0.5,0.5);
  funrth->SetParameters(1e4,0,1e-1);
  funrth->SetParNames("Constant","Mean","Sigma");
  
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
  hRecGEANER->Draw();
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
  hSeedGEANER->Draw();
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
  cout<<"Number of track-candidate with #theta from [30-50] mrad = "<<neff<<endl;
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
  hResTheta->Write();
  hangMCRec->Write();
  hResPointX->Write();
  hResPointY->Write();
  hResPointZ->Write();
  hPointXY->Write();
  hPointXYcut->Write();
  hSeedGEANEX->Write();
  hSeedGEANEY->Write();
  hSeedGEANER->Write();
  hSeedGEANETheta->Write();
  hSeedGEANEPhi->Write();
  hSeedThetaPhi->Write();
  hchi2Errx->Write();
  hchi2Erry->Write();
  
  TCanvas *c10 = new TCanvas("ThetaPhiDistr");
  c10->Divide(2,3);
  c10->cd(1);
  hSeedThetaDiffIDs->Draw();
  c10->cd(2);
  hSeedPhiDiffIDs->Draw();
  c10->cd(3);
  hMCThetaGEANETheta->Draw();
  c10->cd(4);
  hMCPhiGEANEPhi->Draw();
  c10->cd(5);
  hMCThetaResTheta->Draw();
  c10->cd(6);
  hMCPhiResPhi->Draw();
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
  hResLumiTrkMom->Draw();
  c12->cd(2);
  hResLumiTrkTheta->Draw();
  c12->cd(3);
  hResLumiTrkPhi->Draw();
  c12->cd(4);
  hResLumiTrkPointX->Draw();
  c12->cd(5);
  hResLumiTrkPointY->Draw();
  c12->cd(6);
  hResLumiTrkPointZ->Draw();
  c12->cd(7);
  hResLumiTrkPointPx->Draw();
  c12->cd(8);
  hResLumiTrkPointPy->Draw();
  c12->cd(9);
  hResLumiTrkPointPz->Draw();
  c12->Write();
  c12->Close();

 TCanvas *c13 = new TCanvas("TrkLin_and_MC_pulls");
 c13->Divide(3,2);
 c13->cd(1);
 hResLumiTrkPointXPull->Draw();
 c13->cd(2);
 hResLumiTrkPointYPull->Draw();
 c13->cd(3);
 hResLumiTrkPointZPull->Draw();
 c13->cd(4);
 hResLumiTrkPointPxPull->Draw();
 c13->cd(5);
 hResLumiTrkPointPyPull->Draw();
 c13->cd(6);
 hResLumiTrkPointPzPull->Draw();
 c13->Write();
 c13->Close();

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
  hMomMC->Write();
  hResZResPhi->Write();
  f->Close();
}
