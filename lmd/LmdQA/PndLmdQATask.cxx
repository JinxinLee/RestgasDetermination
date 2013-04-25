//-----------------------------------------------------------
//
// Description:
//      Task to generate and store some useful histograms
//
// Author List:
//	Anastasia Karavdina
//      Mathias Michel
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndLmdQATask.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndLinTrack.h"
#include "TrackData/PndTrackCand.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "TrackData/PndTrackCandHit.h"
#include "PndTrack.h"
#include "PndSdsClusterPixel.h"
#include "PndSdsDigiPixel.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "FairTrackParH.h"
#include "PndMCTrack.h"
#include <TMath.h>
#include <TVector3.h>
#include <TRandom2.h>
//#include <TStyle.h>
//#include <TCanvas.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include "TH2D.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
using namespace ROOT::Math;
using namespace std;

PndLmdQATask::PndLmdQATask(TString mcHitBranch, TString mcTrkBranch, TString clusterBranch, TString digiBrunch, TString hitBranch, TString trkCandBranch,TString trackBranch, TString geaneBranch, TString outFile, double Plab)
  : FairTask("Histogram creator")
{
  fmcHitName=mcHitBranch;
  fmcTrkName=mcTrkBranch;
  fHitName=hitBranch;
  fClusterName = clusterBranch;
  fDigiName = digiBrunch;
  fTrkCandName = trkCandBranch;
  fTrkName=trackBranch;
  fGeaneName=geaneBranch;
  foutFile=outFile;
  fEvent=0;
  verboseLevel=false;
  fPlab = Plab;
}

PndLmdQATask::~PndLmdQATask()
{
  // cout<<"start... PndLmdQATask::~PndLmdQATask()"<<endl;
  // delete hResMom;
  // delete hErrMom;
  // delete hPullMom;
  // delete hResTheta;
  // delete hErrTheta;
  // delete hPullTheta;
  // delete hResPhi;
  // delete hErrPhi;
  // delete hPullPhi;

  // delete hResPointPx;
  // delete hErrPointPx;
  // delete hPullPointPx;
  
  // delete hResPointPy;
  // delete hErrPointPy;
  // delete hPullPointPy;
  
  // delete hResPointPz;
  // delete hErrPointPz;
  // delete hPullPointPz;

  // delete hResPointX;
  // delete hPullPointX;
  // delete hResPointY;
  // delete hPullPointY;
  // delete hResPointZ;
  // delete hPullPointZ;

  // delete hhits;
  // delete hchi2;
  // delete hResLumiTrkMom;
  // delete hResLumiTrkTheta;
  // delete hResLumiTrkPhi;
  // delete hResLumiTrkPointX;
  // delete hResLumiTrkPointY;
  // delete hResLumiTrkPointZ;

  // delete hResLumiTrkPointPx;
  // delete hResLumiTrkPointPy;
  // delete hResLumiTrkPointPz;

  // delete hResLumiTrkPointXErr;
  // delete hResLumiTrkPointYErr;
  // delete hResLumiTrkPointZErr;
  // delete hResLumiTrkPointPxErr;
  // delete hResLumiTrkPointPyErr;
  // delete hResLumiTrkPointPzErr;

  // delete hResLumiTrkPointXPull;
  // delete hResLumiTrkPointYPull;
  // delete hResLumiTrkPointZPull;

  // delete hResLumiTrkPointPxPull;
  // delete hResLumiTrkPointPyPull;
  // delete hResLumiTrkPointPzPull;
  // delete hResLumiTrkThetaPull;
  // delete hResLumiTrkPhiPull;

   cout<<"PndLmdQATask::~PndLmdQATask().. finished"<<endl;
}

void PndLmdQATask::WriteHists(){

  // //Write histos
  TFile *f = new TFile(foutFile,"RECREATE");
  f->Print();

  //OutputResolutionAndPulls Directory
  f->mkdir("NearIP");
  f->cd("NearIP");

  hResMom->Write();
  //  hErrMom->Write();
  //  hPullMom->Write();
  hResTheta->Write();
  hErrTheta->Write();
  hPullTheta->Write();
  hResPhi->Write();
  hErrPhi->Write();
  hPullPhi->Write();

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
  hPullPointX->Write();
  hResPointY->Write();
  hPullPointY->Write();
  hResPointZ->Write();
  hPullPointZ->Write();
  f->cd();

  f->mkdir("NearLMD");
  f->cd("NearLMD");
  //Near 1st LMD plane
  hchi2->Write();
  hhits->Write();
  hResLumiTrkMom->Write();
  hResLumiTrkTheta->Write();
  hResLumiTrkPhi->Write();
  hResLumiTrkPointX->Write();
  hResLumiTrkPointY->Write();
  hResLumiTrkPointZ->Write();

  hResLumiTrkPointPx->Write();
  hResLumiTrkPointPy->Write();
  hResLumiTrkPointPz->Write();
  hResLumiTrkPointXErr->Write();
  hResLumiTrkPointYErr->Write();
  hResLumiTrkPointZErr->Write();
  hResLumiTrkPointPxErr->Write();
  hResLumiTrkPointPyErr->Write();
  hResLumiTrkPointPzErr->Write();

  hResLumiTrkPointXPull->Write();
  hResLumiTrkPointYPull->Write();
  hResLumiTrkPointZPull->Write();

  hResLumiTrkPointPxPull->Write();
  hResLumiTrkPointPyPull->Write();
  hResLumiTrkPointPzPull->Write();
  hResLumiTrkThetaPull->Write();
  hResLumiTrkPhiPull->Write();
  f->cd();

  //RecoReso Directory
  f->mkdir("Pulls");
  f->cd("Pulls");

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
  f->cd();
  f->Write();
  f->Close();
  std::cout<<"PndLmdQATask::WriteHists() Finished successfull"<<std::endl;
}

void PndLmdQATask::FinishTask()
{
  // cout<<"PndLmdQATask::FinishTask()"<<endl;

  cout<<"Number of missed tracks is "<<mistrk<<" and number of ghost tracks is "<<ghosttrk<<endl;
  WriteHists();

  //clean up stuff
  // delete c1;
  // delete c2;
 
}

InitStatus PndLmdQATask::Init()
{
  tot = 0;
  all=0; uneff=0; mistrk=0; ghosttrk=0;

  //  fouthists = new TFile(foutFile,"RECREATE");

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndLmdQATask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fmcHitArray=(TClonesArray*) ioman->GetObject(fmcHitName);
  if(fmcHitArray==0)
    {
      Error("PndLmdQATask::Init","mcHit-array not found!");
      return kERROR;
    }

  fHitArray=(TClonesArray*) ioman->GetObject(fHitName);
  if(fHitArray==0)
    {
      Error("PndLmdQATask::Init","hit-array not found!");
      return kERROR;
    }

  fTrkArray=(TClonesArray*) ioman->GetObject(fTrkName);
  if(fTrkArray==0)
    {
      Error("PndLmdQATask::Init","track-array not found!");
      return kERROR;
    }

  fmcTrkArray=(TClonesArray*) ioman->GetObject(fmcTrkName);
  if(fmcTrkArray==0)
    {
      Error("PndLmdQATask::Init","mcTrk-array not found!");
      return kERROR;
    }

  fGeaneArray=(TClonesArray*) ioman->GetObject(fGeaneName);
  if(fGeaneArray==0)
    {
      Error("PndLmdQATask::Init","geane-array not found!");
      return kERROR;
    }
  fTrkCandArray=(TClonesArray*) ioman->GetObject(fTrkCandName);
  if(fTrkCandArray==0)
    {
      Error("PndLmdQATask::Init","trk-cand--array not found!");
      return kERROR;
    }
  fClusterArray=(TClonesArray*) ioman->GetObject(fClusterName);
  if(fClusterArray==0)
    {
      Error("PndLmdQATask::Init","cluster-array not found!");
      return kERROR;
    }
  fDigiArray=(TClonesArray*) ioman->GetObject(fDigiName);
  if(fDigiArray==0)
    {
      Error("PndLmdQATask::Init","digi-array not found!");
      return kERROR;
    }
  double thetarange[2]={0.001,0.01};
  double thetam =  thetarange[0];
  if(fPlab<5) thetam =  thetarange[1];
  //Near IP
  hResMom = new TH1F("hResMom","P_{MC}-P_{REC};#deltaP,GeV/c",1e3,-1e-4,1e-4);
  //  hErrMom = new TH1F("hErrMom","#sigma_{P};#sigmaP,GeV/c",1e3,0,1e-3);
  //  hPullMom = new TH1F("hPullMom","(P_{MC}-P_{REC})/#sigma_{P};",1e3,-1e1,1e1);
  hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{REC};#delta#theta,rad",1e2,-thetam,thetam);//TEST
  hErrTheta = new TH1F("hErrTheta","#sigma(#theta_{REC});#sigma,rad",1e3,0,10*thetam);
   hPullTheta = new TH1F("hPullTheta","(#theta_{MC}-#theta_{REC})/#sigma_{#theta};",1e2,-10,10);
   hResPhi = new TH1F("hResPhi","#phi_{MC}-#phi_{REC};#delta#phi,rad",2e3,-1.,1.);
   hErrPhi = new TH1F("hErrPhi","#sigma(#phi_{REC});#sigma,rad",1e3,0,0.1);
   hPullPhi = new TH1F("hPullPhi","(#phi_{MC}-#phi_{REC})/#sigma_{#phi};",1e2,-10,10);

   hResPointPx = new TH1F("hResPointPx","Px_{MC}-Px_{REC};#deltaPx, GeV/c",1e2,-0.01,0.01);
   hErrPointPx = new TH1F("hErrPointPx","#sigma_{Px};#sigmaPx, GeV/c",1e3,0,0.01);
   hPullPointPx = new TH1F("hPullPointPx","(Px_{MC}-Px_{REC})/#sigma_{Px};(Px_{MC}-Px_{REC})/#sigma_{Px}",1e2,-10,10);
  
   hResPointPy = new TH1F("hResPointPy","Py_{MC}-Py_{REC};#deltaPy, GeV/c",1e2,-0.01,0.01);
   hErrPointPy = new TH1F("hErrPointPy","#sigma_{Py};#sigmaPy, GeV/c",1e3,0,0.01);
   hPullPointPy = new TH1F("hPullPointPy","(Py_{MC}-Py_{REC})/#sigma_{Py};(Py_{MC}-Py_{REC})/#sigma_{Py}",1e2,-10,10);
  
   hResPointPz = new TH1F("hResPointPz","Pz_{MC}-Pz_{REC};#deltaPz, GeV/c",1e2,-1e-3,1e-3);
   hErrPointPz = new TH1F("hErrPointPz","#sigma_{Pz};#sigmaPz, GeV/c",1e3,0,1e-1);
   hPullPointPz = new TH1F("hPullPointPz","(Pz_{MC}-Pz_{REC})/#sigma_{Pz};(Pz_{MC}-Pz_{REC})/#sigma_{Pz}",1e2,-10,10);

   hResPointX = new TH1F("hResPointX","X_{MC}-X_{REC};#deltaX,cm",1e2,-2.,2.);
   hPullPointX = new TH1F("hPullPointX","(X_{MC}-X_{REC})/#sigma_{X};(X_{MC}-X_{REC})/#sigma_{X}",1e2,-10,10);
   hResPointY = new TH1F("hResPointY","Y_{MC}-Y_{REC};#deltaY,cm",1e2,-2.,2.);
   hPullPointY = new TH1F("hPullPointY","(Y_{MC}-Y_{REC})/#sigma_{Y};(Y_{MC}-Y_{REC})/#sigma_{Y}",1e2,-10,10);
   hResPointZ = new TH1F("hResPointZ","Z_{MC}-Z_{REC};#deltaZ,cm",1e3,-0.15,0.15);
   hPullPointZ = new TH1F("hPullPointZ","(Z_{MC}-Z_{REC})/#sigma_{Z};(Z_{MC}-Z_{REC})/#sigma_{Z}",1e2,-10,10);

  //Near 1st LMD plane
   hhits = new TH1I("hhits","number of hits in trk",7,0,7);
   hchi2 = new TH1F("hchi2","#chi^2 for reconstructed tracks;#chi^2;",1.5e2,0,15.);
   hResLumiTrkMom = new TH1F("hResLumiTrkMom","P_{MC}-P_{REC}(near Lumi);#deltaP,GeV/c",1e3,-6e-7,6e-7);
   hResLumiTrkTheta = new TH1F("hResLumiTrkTheta","#theta_{MC}-#theta_{REC}(near Lumi);#delta#theta,rad",1e3,-6e-3,6e-3);
   hResLumiTrkPhi = new TH1F("hResLumiTrkPhi","#phi_{MC}-#phi_{REC}(near Lumi);#delta#phi,rad",2e3,-1e-1,1e-1);
   hResLumiTrkPointX = new TH1F("hResLumiTrkPointX","X_{MC}-X_{REC}(near Lumi);#deltaX,cm",1e2,-0.02,0.02);
   hResLumiTrkPointY = new TH1F("hResLumiTrkPointY","Y_{MC}-Y_{REC}(near Lumi);#deltaY,cm",1e2,-0.02,0.02);
   hResLumiTrkPointZ = new TH1F("hResLumiTrkPointZ","Z_{MC}-Z_{REC}(near Lumi);#deltaZ,cm",1e2,-0.02,0.02);

   hResLumiTrkPointPx = new TH1F("hResLumiTrkPointPx","Px_{MC}-Px_{REC}(near Lumi);#deltaPx, GeV/c",1e2,-0.01,0.01);
   hResLumiTrkPointPy = new TH1F("hResLumiTrkPointPy","Py_{MC}-Py_{REC}(near Lumi);#deltaPy, GeV/c",1e2,-0.01,0.01);
   hResLumiTrkPointPz = new TH1F("hResLumiTrkPointPz","Pz_{MC}-Pz_{REC}(near Lumi);#deltaPz, GeV/c",1e2,-0.1,0.1);

   hResLumiTrkPointXErr = new TH1F("hResLumiTrkPointXErr","#sigma(X_{REC})(near Lumi);#sigma_{X},cm",1e2,0,0.02);
   hResLumiTrkPointYErr = new TH1F("hResLumiTrkPointYErr","#sigma(Y_{REC})(near Lumi);#sigma_{Y},cm",1e2,0,0.02);
   hResLumiTrkPointZErr = new TH1F("hResLumiTrkPointZErr","#sigma(Z_{REC})(near Lumi);#sigma_{Z},cm",1e2,0,0.02);
   hResLumiTrkPointPxErr = new TH1F("hResLumiTrkPointPxErr","#sigma(Px_{REC})(near Lumi);#sigma_{Px}, GeV/c",1e2,0,0.01);
   hResLumiTrkPointPyErr = new TH1F("hResLumiTrkPointPyErr","#sigma(Py_{REC})(near Lumi);#sigma_{Py}, GeV/c",1e2,0,0.01);
   hResLumiTrkPointPzErr = new TH1F("hResLumiTrkPointPzErr","#sigma(Pz_{REC})(near Lumi);#sigma_{Pz}, GeV/c",1e2,0,0.001);

   hResLumiTrkPointXPull = new TH1F("hResLumiTrkPointXPull","(X_{MC}-X_{REC})/#sigma (near Lumi) ;(X_{MC}-X_{REC})/#sigma",1e2,-10.,10.);
   hResLumiTrkPointYPull = new TH1F("hResLumiTrkPointYPull","(Y_{MC}-Y_{REC})/#sigma (near Lumi);(Y_{MC}-Y_{REC})/#sigma",1e2,-10.,10.);
   hResLumiTrkPointZPull = new TH1F("hResLumiTrkPointZPull","(Z_{MC}-Z_{REC})/#sigma (near Lumi);(Z_{MC}-Z_{REC})/#sigma",1e3,-100.,100.);

   hResLumiTrkPointPxPull = new TH1F("hResLumiTrkPointPxPull","(Px_{MC}-Px_{REC})/#sigma (near Lumi);(Px_{MC}-Px_{REC})/#sigma",1e2,-10,10.);
   hResLumiTrkPointPyPull = new TH1F("hResLumiTrkPointPyPull","(Py_{MC}-Py_{REC})/#sigma (near Lumi);(Py_{MC}-Py_{REC})/#sigma",1e2,-10,10);
   hResLumiTrkPointPzPull = new TH1F("hResLumiTrkPointPzPull","(Pz_{MC}-Pz_{REC})/#sigma (near Lumi);(Pz_{MC}-Pz_{REC})/#sigma",1e2,-10,10);
   hResLumiTrkThetaPull = new TH1F("hResLumiTrkThetaPull","(#theta_{MC}-#theta_{REC})/#sigma (near Lumi);#delta#theta, rad",1e2,-10,10);
   hResLumiTrkPhiPull = new TH1F("hResLumiTrkPhiPull","(#phi_{MC}-#phi_{REC})/#sigma (near Lumi);#delta#phi, rad",1e2,-10,10);


  std::cout << "-I- PndLmdQATask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

void PndLmdQATask::Exec(Option_t* opt)
{
  //  std::cout<<"PndLmdQATask::Exec"<<std::endl;

  ResoAndPulls();
  // HitReco();
  return;
}

void PndLmdQATask::HitReco()
{
    // const int nMCHits = fmcHitArray->GetEntriesFast();
    // const int nRecHits = fHitArray->GetEntriesFast();

    // for (Int_t i=0; i<nRecHits; i++){
    //   PndSdsHit *hit = (PndSdsHit*) fHitArray->At(i);
    //   if(hit->GetRefIndex()<0) continue;
    //   PndSdsMCPoint *mc = (PndSdsMCPoint*) fmcHitArray->At(hit->GetRefIndex());
    //   if(!mc) continue;

    //   TVector3 recovec(hit->GetX(),hit->GetY(),hit->GetZ());
    //   TVector3 mcvec(mc->GetX()+mc->GetXOut(),mc->GetY()+mc->GetYOut(),mc->GetZ()+mc->GetZOut()); mcvec*=0.5;
    //   TVector3 dvec(recovec-mcvec);

    //   if(hit->GetZ()<1135.) //Acc of first plane
    // 	h2dPnts->Fill(mc->GetX(),mc->GetY());  // = new TH2D("h2dPnts", "xy reconstructed points, cm",2e2,-20.,20., 2e2,-20.,20.);
  
    //   hResHitX->Fill(dvec.X());  // = new TH1F("hResPointX","X_{MC}-X_{rec};#deltaX,cm",2e2,-1.5,1.5);
    //   hResHitY->Fill(dvec.Y());  // = new TH1F("hResPointY","Y_{MC}-Y_{rec};#deltaY,cm",2e2,-1.5,1.5);
    //   hResHitZ->Fill(dvec.Z());  // = new TH1F("hResPointZ","Z_{MC}-Z_{rec};#deltaZ,cm",2e2,-3e-2,3e-2);
    //   //TODO
    //   hErrHitX->Fill(0);  // = new TH1F("hErrPointX","#sigma_{X};#sigmaX,cm",1e2,0,5e-1);
    //   hErrHitY->Fill(0);  // = new TH1F("hErrPointY","#sigma_{Y};#sigmaY,cm",1e2,0,5e-1);
    //   hErrHitZ->Fill(0);  // = new TH1F("hErrPointZ","#sigma_{Z};#sigmaZ,cm",1e2,0,5e-3);
    //   hPullHitX->Fill(0);  // = new TH1F("hPullPointX","(X_{MC}-X_{rec})/#sigma_{X};",1e2,-10,10);
    //   hPullHitY->Fill(0);  // = new TH1F("hPullPointY","(Y_{MC}-Y_{rec})/#sigma_{Y};",1e2,-10,10);
    //   hPullHitZ->Fill(0);  // = new TH1F("hPullPointZ","(Z_{MC}-Z_{rec})/#sigma_{Z};",1e2,-10,10);

    // }
    // //----------------------------------------------------------------------------------
    return;
}

void PndLmdQATask::ResoAndPulls()
{
    fEvent++;
    // Read GEANE & MC info -----------------------------------------------------------------
    const int nGeaneTrks = fGeaneArray->GetEntriesFast();
    const int nParticles = fmcTrkArray->GetEntriesFast();
    const int nRecHits = fHitArray->GetEntriesFast();
    const int nRecTrks = fTrkArray->GetEntriesFast();

    if(nGeaneTrks<nParticles)
      mistrk += nParticles-nGeaneTrks;
    
    if(nGeaneTrks>nParticles)
      ghosttrk += nGeaneTrks-nParticles;
    if(verboseLevel>0) {
      if(nParticles!=1) 
	std::cout<<"Hey, QA task is implemented only for 1 trk/event case. In Ev #"<<fEvent-1<<" you have "<<nParticles<<" MC tracks!"<<std::endl;
    }
    //    if(nParticles!=1) return; //TODO: currently works only 1 trk/event !
    if(verboseLevel>0)  
      cout<<"# "<< fEvent-1 <<"\t nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<" nRecTrks="<<nRecTrks<<endl;
    //TODO: correct assignment between MC and REC trks
    // vector<int> missedTrk;
    // missedTrk.resize(nParticles);
    // vector<int> ghostTrk;
    // ghostTrk.resize(nGeaneTrks);
    
 for (Int_t iN=0; iN<nGeaneTrks; iN++){// loop over all reconstructed trks
      FairTrackParH *fRes = (FairTrackParH*)fGeaneArray->At(iN);
      Double_t lyambda = fRes->GetLambda();
      if(lyambda==0){
	cout<<"GEANE didn't propagate this trk!"<<endl;
	//	glBADGEANE++;
      }
      if(lyambda==0) continue;

      /// Read REC track parameters near IP -----------------------------------
      TVector3 MomRecPCA = fRes->GetMomentum();
      MomRecPCA *= fPlab/MomRecPCA.Mag();
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
      // if(fPlab<5) pca_lim = 2.;
      // if(fPlab<2) pca_lim = 5.;
      // if(fabs(PosRecPCA.X())>pca_lim && fabs(PosRecPCA.Y())>pca_lim) continue; // PCA_x and PCA_y should be < 10sigmaX
      // ///get rid from most probably ghost track (END) ---     
      // ///------------------------------------------------------------------------------------

      /// Read REC track parameters near LMD -----------------------------------
      PndTrack *trkpnd = (PndTrack*)fTrkArray->At(iN);
      double chi2 = trkpnd->GetChi2();
      hchi2->Fill(chi2);
      FairTrackParP fFittedTrkP = trkpnd->GetParamFirst();
      TVector3 PosRecLMD(fFittedTrkP.GetX(),fFittedTrkP.GetY(),fFittedTrkP.GetZ());
      TVector3 MomRecLMD(fFittedTrkP.GetPx(),fFittedTrkP.GetPy(),fFittedTrkP.GetPz());
      MomRecLMD *=fPlab/MomRecLMD.Mag();
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
      PndTrackCand *trkcand = (PndTrackCand*)fTrkCandArray->At(candID);    
      const int Ntrkcandhits= trkcand->GetNHits();
      PndSdsMCPoint* MCPointHit;
      int MCid;
      bool hitmix = false;
      if(Ntrkcandhits<4) continue; //require trks with hits on all planes
      hhits->Fill(Ntrkcandhits);
      for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){ // loop over rec.hits
	PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	Int_t hitID = candhit.GetHitId();
	PndSdsHit* myHit = (PndSdsHit*)(fHitArray->At(hitID));
	
	//for pixel design
	PndSdsClusterPixel* myCluster = (PndSdsClusterPixel*)(fClusterArray->At(myHit->GetClusterIndex()));
	PndSdsDigiPixel* astripdigi = (PndSdsDigiPixel*)(fDigiArray->At(myCluster->GetDigiIndex(0)));
	if (astripdigi->GetIndex(0) == -1)
	  continue;
	PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(fmcHitArray->At(astripdigi->GetIndex(0)));
	int MCidTOP = MCPoint->GetTrackID();
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
	PndMCTrack *mctrk =(PndMCTrack*) fmcTrkArray->At(MCid);
	Int_t mcID = mctrk->GetPdgCode();
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
	hResPhi->Fill(MomMCpca.Phi()-MomRecPCA.Phi());
	hPullTheta->Fill((MomMCpca.Theta()-MomRecPCA.Theta())/err_lyambda);
	hPullPhi->Fill((MomMCpca.Phi()-MomRecPCA.Phi())/err_phi);
	hErrTheta->Fill(err_lyambda);
	hErrPhi->Fill(err_phi);
	hResMom->Fill(MomMCpca.Mag()-MomRecPCA.Mag());
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
	MomMClmd = dirMClmd*fPlab;
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

  return;
}

ClassImp(PndLmdQATask);
